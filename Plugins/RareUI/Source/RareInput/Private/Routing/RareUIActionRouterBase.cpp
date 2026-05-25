
#include "Routing/RareUIActionRouterBase.h"

#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Device/RareAnalogCursor.h"
#include "Device/RareInputPreprocessor.h"
#include "CommonUI/Private/Input/UIActionRouterTypes.h"
#include "Algo/AnyOf.h"
#include "Core/RareInputAction.h"

URareUIActionRouterBase* URareUIActionRouterBase::Get(const UWidget& ContextWidget)
{
	return Cast<URareUIActionRouterBase>(UCommonUIActionRouterBase::Get(ContextWidget));
}

void URareUIActionRouterBase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AnalogCursor = StaticCastSharedPtr<FRareAnalogCursor, FCommonAnalogCursor>(GetCommonAnalogCursor());
	RareInputPreprocessor = MakeShared<FRareInputPreprocessor>();
	FSlateApplication::Get().RegisterInputPreProcessor(RareInputPreprocessor, 0);

	// Listen for player controller changes so we can rebind Enhanced Input bindings to the new component.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		LocalPlayer->OnPlayerControllerChanged().AddUObject(this, &URareUIActionRouterBase::HandlePlayerControllerChanged);
	}
}

void URareUIActionRouterBase::Deinitialize()
{
	// Unbind the PC change delegate.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		LocalPlayer->OnPlayerControllerChanged().RemoveAll(this);
	}
	
	// Remove all Enhanced Input bindings before we go away.
	UnbindAllFromEnhancedInputComponent();
	for (const FRareActionBindingHandle& Handle : AllActionBindings)
	{
		FRareActionBinding::RemoveBinding(Handle);
	}
	AllActionBindings.Empty();
	ActiveBindings.Empty();

	if (RareInputPreprocessor.IsValid())
	{
		RareInputPreprocessor->GamepadValuesHandlers.Reset();
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(RareInputPreprocessor);
		}
		RareInputPreprocessor.Reset();
	}


	Super::Deinitialize();
}

// ── Analog cursor ─────────────────────────────────────────────────────────────

void URareUIActionRouterBase::SetAnalogMovementEnabled(const bool bEnable) const
{
	AnalogCursor->SetAnalogMovementEnabled(bEnable);
}

TSharedRef<FCommonAnalogCursor> URareUIActionRouterBase::MakeAnalogCursor() const
{
	return FCommonAnalogCursor::CreateAnalogCursor<FRareAnalogCursor>(*this);
}

// ── Gamepad values listeners ──────────────────────────────────────────────────

void URareUIActionRouterBase::AddGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const
{
	if (RareInputPreprocessor.IsValid())
	{
		RareInputPreprocessor->GamepadValuesHandlers.Add(InHandler);
	}
}

void URareUIActionRouterBase::RemoveGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const
{
	if (RareInputPreprocessor.IsValid() && RareInputPreprocessor->GamepadValuesHandlers.Contains(InHandler))
	{
		RareInputPreprocessor->GamepadValuesHandlers.Remove(InHandler);
	}
}

// ── Enhanced Input registration ───────────────────────────────────────────────

FRareActionBindingHandle URareUIActionRouterBase::RegisterInputAction(const UWidget& Widget, const FRareInputActionBindingArgs& BindActionArgs)
{
	if (!ensureMsgf(IsValid(BindActionArgs.Data.InputAction), TEXT("[URareUIActionRouterBase::RegisterInputAction] InputAction is null. Widget: %s"), *Widget.GetName()))
	{
		return FRareActionBindingHandle{};
	}

	const int32 NewId = NextBindingId++;
	FRareActionBindingHandle NewHandle(NewId);
	NewHandle.Router = this;

	FRareActionBinding NewBinding;
	
	NewBinding.BindingId = NewId;
	NewBinding.Handle = NewHandle;
	NewBinding.RegisteringWidget = &Widget;
	NewBinding.OwningActivatable = ResolveOwningActivatable(Widget);
	NewBinding.InputAction = BindActionArgs.Data.InputAction;
	NewBinding.TriggerEvent = BindActionArgs.TriggerEvent;
	NewBinding.Callback = BindActionArgs.Callback;
	NewBinding.bDisplayInActionBar = BindActionArgs.Data.bDisplayInActionBar;
	NewBinding.DisplayName = BindActionArgs.Data.DisplayName;
	NewBinding.ActionBarPriority = BindActionArgs.Data.ActionBarPriority;
	
	FRareActionBinding::AddBinding(MoveTemp(NewBinding));
	AllActionBindings.Add(NewHandle);
	
	RefreshActiveBindingsCache();

	// If the Enhanced Input Component is already available, subscribe immediately.
	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		EnsureUniqueEIBinding(BindActionArgs.Data.InputAction, BindActionArgs.TriggerEvent, EnhancedInputComponent);
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("[RegisterInputAction] EnhancedInputComponent not available yet for action '%s'. Will bind when the player controller changes."), *BindActionArgs.Data.InputAction->GetName());
	}

	return NewHandle;
}

const TArray<FRareActionBindingHandle>& URareUIActionRouterBase::GetActiveBindings() const
{
	return ActiveBindings;
}

void URareUIActionRouterBase::UnregisterInputAction(FRareActionBindingHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	const TSharedPtr<FRareActionBinding> Found = FRareActionBinding::FindBinding(Handle);
	if (!Found)
	{
		AllActionBindings.Remove(Handle);
		Handle.BindingId = INDEX_NONE;
		Handle.Router.Reset();
		return;
	}

	UInputAction* RemovedAction = Found->InputAction.Get();
	const ETriggerEvent RemovedEvent = Found->TriggerEvent;
	FRareActionBinding::RemoveBinding(Handle);
	AllActionBindings.Remove(Handle);
	
	Handle.BindingId = INDEX_NONE;
	Handle.Router.Reset();
	
	RefreshActiveBindingsCache();

	// If no other binding still needs that (action, event) pair, remove the Enhanced Input binding too.
	const bool bStillNeeded = Algo::AnyOf(AllActionBindings, [RemovedAction, RemovedEvent](const FRareActionBindingHandle& ExistingHandle)
	{
		if (const TSharedPtr<FRareActionBinding> ExistingBinding = FRareActionBinding::FindBinding(ExistingHandle))
		{
			return ExistingBinding->InputAction == RemovedAction && ExistingBinding->TriggerEvent == RemovedEvent;
		}

		return false;
	});

	if (!bStillNeeded)
	{
		RemoveUniqueEIBinding(RemovedAction, RemovedEvent);
	}
}

FUIActionBindingHandle URareUIActionRouterBase::RegisterUIActionBinding(const UWidget& Widget, const FBindUIActionArgs& BindActionArgs)
{
	ensureAlwaysMsgf(false, TEXT("We're bypassing CommonInput entirely, use RegisterInputAction function instead. Widget: %s"), *Widget.GetName());
	return FUIActionBindingHandle();
}

void URareUIActionRouterBase::SetActiveRoot(const FActivatableTreeRootPtr NewActiveRoot)
{
	// Let CommonUI do its work (it will bind its own handler to OnLeafmostActiveNodeChanged via BindUObject).
	Super::SetActiveRoot(NewActiveRoot);

	// Overwrite that binding with our own so we also refresh our cache when the leafmost node changes.
	// We must replicate what CommonUI's handler does (broadcast OnBoundActionsUpdated) since we replaced it.
	if (NewActiveRoot.IsValid())
	{
		NewActiveRoot->OnLeafmostActiveNodeChanged.BindUObject(this, &URareUIActionRouterBase::HandleLeafmostActiveNodeChangedInternal);
	}

	RefreshActiveBindingsCache();
}

UEnhancedInputComponent* URareUIActionRouterBase::GetEnhancedInputComponent() const
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>();
	if (!IsValid(LocalPlayer))
	{
		return nullptr;
	}

	const APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}

	return Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
}

void URareUIActionRouterBase::EnsureUniqueEIBinding(const UInputAction* Action, const ETriggerEvent Event, UEnhancedInputComponent* EnhancedInputComponent)
{
	if (UniqueEnhancedInputHandles.Contains({Action, Event}))
	{
		return;
	}

	const FEnhancedInputActionEventBinding& Binding = EnhancedInputComponent->BindAction(Action, Event, this, &URareUIActionRouterBase::ProcessEnhancedInputAction);
	UniqueEnhancedInputHandles.Add({Action, Event}, Binding.GetHandle());
}

void URareUIActionRouterBase::RemoveUniqueEIBinding(const UInputAction* Action, const ETriggerEvent Event)
{
	const uint32* Handle = UniqueEnhancedInputHandles.Find({Action, Event});
	if (!Handle)
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		EnhancedInputComponent->RemoveBindingByHandle(*Handle);
	}

	UniqueEnhancedInputHandles.Remove({Action, Event});
}

void URareUIActionRouterBase::BindAllToEnhancedInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	for (const FRareActionBindingHandle& BindingHandle : AllActionBindings)
	{
		if (const TSharedPtr<FRareActionBinding> Binding = FRareActionBinding::FindBinding(BindingHandle); Binding && IsValid(Binding->InputAction.Get()))
		{
			EnsureUniqueEIBinding(Binding->InputAction.Get(), Binding->TriggerEvent, EnhancedInputComponent);
		}
	}
}

void URareUIActionRouterBase::UnbindAllFromEnhancedInputComponent()
{
	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		for (const auto& [Key, Handle] : UniqueEnhancedInputHandles)
		{
			EnhancedInputComponent->RemoveBindingByHandle(Handle);
		}
	}

	UniqueEnhancedInputHandles.Empty();
}

void URareUIActionRouterBase::HandlePlayerControllerChanged(APlayerController* /*NewController*/)
{
	// Stale handles belong to the old component — discard and rebind to the new one.
	UniqueEnhancedInputHandles.Empty();

	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		BindAllToEnhancedInputComponent(EnhancedInputComponent);
	}

	RefreshActiveBindingsCache();
}

void URareUIActionRouterBase::HandleLeafmostActiveNodeChangedInternal()
{
	// Replicate what UCommonUIActionRouterBase::HandleLeafmostActiveNodeChanged does.
	OnBoundActionsUpdated().Broadcast();

	// Eagerly refresh active bindings so dispatch only performs action/event filtering.
	RefreshActiveBindingsCache();
}

void URareUIActionRouterBase::ProcessEnhancedInputAction(const FInputActionInstance& ActionInstance)
{
	const UInputAction* InputAction = ActionInstance.GetSourceAction().Get();
	const ETriggerEvent TriggerEvent = ActionInstance.GetTriggerEvent();

	if (ActiveBindings.IsEmpty())
	{
		return;
	}

	// Work on a copy so callbacks can safely call UnregisterInputAction.
	const TArray<FRareActionBindingHandle> BindingHandlesCopy = GetActiveBindings();
	for (const FRareActionBindingHandle& BindingHandle : BindingHandlesCopy)
	{
		const TSharedPtr<FRareActionBinding> Binding = FRareActionBinding::FindBinding(BindingHandle);
		if (!Binding)
		{
			continue;
		}

		if (Binding->InputAction != InputAction || Binding->TriggerEvent != TriggerEvent)
		{
			continue;
		}

		Binding->Callback.ExecuteIfBound();
	}
}

UCommonActivatableWidget* URareUIActionRouterBase::ResolveOwningActivatable(const UWidget& Widget) const
{
	if (UCommonActivatableWidget* AsActivatable = Cast<UCommonActivatableWidget>(const_cast<UWidget*>(&Widget)))
	{
		return AsActivatable;
	}

	const TSharedPtr<SWidget> CachedWidget = Widget.GetCachedWidget();
	ULocalPlayer* OwningLocalPlayer = Widget.GetOwningLocalPlayer();
	return FindOwningActivatable(CachedWidget, OwningLocalPlayer);
}

void URareUIActionRouterBase::RefreshActiveBindingsCache()
{
	ActiveBindings.Empty();

	for (auto It = AllActionBindings.CreateIterator(); It; ++It)
	{
		const FRareActionBindingHandle BindingHandle = *It;
		const TSharedPtr<FRareActionBinding> Binding = FRareActionBinding::FindBinding(BindingHandle);
		if (!Binding)
		{
			It.RemoveCurrent();
			continue;
		}

		if (!IsValid(Binding->RegisteringWidget.Get()) || !IsValid(Binding->InputAction.Get()))
		{
			FRareActionBinding::RemoveBinding(BindingHandle);
			It.RemoveCurrent();
			continue;
		}

		if (!IsBindingActive(*Binding))
		{
			continue;
		}

		ActiveBindings.Add(BindingHandle);
	}
}

bool URareUIActionRouterBase::IsBindingActive(const FRareActionBinding& Binding) const
{
	if (const UCommonActivatableWidget* OwningActivatable = Binding.OwningActivatable.Get())
	{
		return IsWidgetInActiveRoot(OwningActivatable) || IsWidgetInLeafmostNodeHierarchy(*OwningActivatable);
	}

	if (const UWidget* RegisteringWidget = Binding.RegisteringWidget.Get())
	{
		return IsWidgetInLeafmostNodeHierarchy(*RegisteringWidget);
	}

	return false;
}
