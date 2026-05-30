#include "Routing/EazyUIActionRouterBase.h"

#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Device/EazyAnalogCursor.h"
#include "CommonUI/Private/Input/UIActionRouterTypes.h"
#include "Algo/AnyOf.h"
#include "EazyInputAction.h"
#include "LogEazyUIInput.h"

UEazyUIActionRouterBase* UEazyUIActionRouterBase::Get(const UWidget& ContextWidget)
{
	return Cast<UEazyUIActionRouterBase>(UCommonUIActionRouterBase::Get(ContextWidget));
}

void UEazyUIActionRouterBase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AnalogCursor = StaticCastSharedPtr<FEazyAnalogCursor, FCommonAnalogCursor>(GetCommonAnalogCursor());

	// Listen for player controller changes so we can rebind Enhanced Input bindings to the new component.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		LocalPlayer->OnPlayerControllerChanged().AddUObject(this, &UEazyUIActionRouterBase::HandlePlayerControllerChanged);
	}
}

void UEazyUIActionRouterBase::Deinitialize()
{
	// Unbind the PC change delegate.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		LocalPlayer->OnPlayerControllerChanged().RemoveAll(this);
	}
	
	// Remove all Enhanced Input bindings before we go away.
	UnbindAllFromEnhancedInputComponent();
	for (const FEazyActionBindingHandle& Handle : AllActionBindings)
	{
		FEazyActionBinding::RemoveBinding(Handle);
	}
	AllActionBindings.Empty();
	ActiveBindings.Empty();
	ActiveBindingsByInputAction.Empty();


	Super::Deinitialize();
}

void UEazyUIActionRouterBase::SetAnalogMovementEnabled(const bool bEnable) const
{
	if (ensureMsgf(AnalogCursor.IsValid(), TEXT("[SetAnalogMovementEnabled] Analog cursor is invalid.")))
	{
		AnalogCursor->SetAnalogMovementEnabled(bEnable);
	}
}

TSharedRef<FCommonAnalogCursor> UEazyUIActionRouterBase::MakeAnalogCursor() const
{
	return FCommonAnalogCursor::CreateAnalogCursor<FEazyAnalogCursor>(*this);
}

FEazyActionBindingHandle UEazyUIActionRouterBase::RegisterInputAction(const UWidget& Widget, const FEazyInputActionBindingArgs& BindActionArgs)
{
	if (!ensureMsgf(IsValid(BindActionArgs.Data.InputAction), TEXT("[UEazyUIActionRouterBase::RegisterInputAction] InputAction is null. Widget: %s"), *Widget.GetName()))
	{
		return FEazyActionBindingHandle{};
	}

	const int32 NewId = NextBindingId++;
	FEazyActionBindingHandle NewHandle(NewId);
	NewHandle.Router = this;

	FEazyActionBinding NewBinding;
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
	
	FEazyActionBinding::AddBinding(MoveTemp(NewBinding));
	AllActionBindings.Add(NewHandle);
	
	RefreshActiveBindingsCache();

	// If the Enhanced Input Component is already available, subscribe immediately.
	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		EnsureUniqueBinding(BindActionArgs.Data.InputAction, BindActionArgs.TriggerEvent, EnhancedInputComponent);
	}
	else
	{
		UE_LOG(LogEazyUIInput, Verbose, TEXT("[RegisterInputAction] EnhancedInputComponent not available yet for action '%s'. Will bind when the player controller changes."), *BindActionArgs.Data.InputAction->GetName());
	}

	return NewHandle;
}

const TArray<FEazyActionBindingHandle>& UEazyUIActionRouterBase::GetActiveBindings() const
{
	return ActiveBindings;
}

void UEazyUIActionRouterBase::UnregisterInputAction(FEazyActionBindingHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	const TSharedPtr<FEazyActionBinding> Found = FEazyActionBinding::FindBinding(Handle);
	if (!Found)
	{
		AllActionBindings.Remove(Handle);
		Handle.BindingId = INDEX_NONE;
		Handle.Router.Reset();
		return;
	}

	UInputAction* RemovedAction = Found->InputAction.Get();
	const ETriggerEvent RemovedEvent = Found->TriggerEvent;
	FEazyActionBinding::RemoveBinding(Handle);
	AllActionBindings.Remove(Handle);
	
	Handle.BindingId = INDEX_NONE;
	Handle.Router.Reset();
	
	RefreshActiveBindingsCache();

	// If no other binding still needs that (action, event) pair, remove the Enhanced Input binding too.
	const bool bStillNeeded = Algo::AnyOf(AllActionBindings, [RemovedAction, RemovedEvent](const FEazyActionBindingHandle& ExistingHandle)
	{
		if (const TSharedPtr<FEazyActionBinding> ExistingBinding = FEazyActionBinding::FindBinding(ExistingHandle))
		{
			return ExistingBinding->InputAction == RemovedAction && ExistingBinding->TriggerEvent == RemovedEvent;
		}

		return false;
	});

	if (!bStillNeeded)
	{
		RemoveUniqueBinding(RemovedAction, RemovedEvent);
	}
}

FUIActionBindingHandle UEazyUIActionRouterBase::RegisterUIActionBinding(const UWidget& Widget, const FBindUIActionArgs& BindActionArgs)
{
	ensureAlwaysMsgf(false, TEXT("We're bypassing CommonInput entirely, use RegisterInputAction function instead. Widget: %s"), *Widget.GetName());
	return FUIActionBindingHandle();
}

void UEazyUIActionRouterBase::SetActiveRoot(const FActivatableTreeRootPtr NewActiveRoot)
{
	// Let CommonUI do its work (it will bind its own handler to OnLeafmostActiveNodeChanged via BindUObject).
	Super::SetActiveRoot(NewActiveRoot);

	// Overwrite that binding with our own so we also refresh our cache when the leafmost node changes.
	// We must replicate what CommonUI's handler does (broadcast OnBoundActionsUpdated) since we replaced it.
	if (NewActiveRoot.IsValid())
	{
		NewActiveRoot->OnLeafmostActiveNodeChanged.BindUObject(this, &UEazyUIActionRouterBase::HandleLeafmostActiveNodeChangedInternal);
	}

	RefreshActiveBindingsCache();
}

UEnhancedInputComponent* UEazyUIActionRouterBase::GetEnhancedInputComponent() const
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

void UEazyUIActionRouterBase::EnsureUniqueBinding(const UInputAction* Action, const ETriggerEvent Event, UEnhancedInputComponent* EnhancedInputComponent)
{
	const TPair<TObjectKey<const UInputAction>, ETriggerEvent> ActionEventKey(TObjectKey<const UInputAction>(Action), Event);
	if (UniqueEnhancedInputHandles.Contains(ActionEventKey))
	{
		return;
	}

	const FEnhancedInputActionEventBinding& Binding = EnhancedInputComponent->BindAction(Action, Event, this, &UEazyUIActionRouterBase::ProcessEnhancedInputAction);
	UniqueEnhancedInputHandles.Add(ActionEventKey, Binding.GetHandle());
}

void UEazyUIActionRouterBase::RemoveUniqueBinding(const UInputAction* Action, const ETriggerEvent Event)
{
	const TPair<TObjectKey<const UInputAction>, ETriggerEvent> ActionEventKey(TObjectKey<const UInputAction>(Action), Event);
	const uint32* Handle = UniqueEnhancedInputHandles.Find(ActionEventKey);
	if (!Handle)
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		EnhancedInputComponent->RemoveBindingByHandle(*Handle);
	}

	UniqueEnhancedInputHandles.Remove(ActionEventKey);
}

void UEazyUIActionRouterBase::BindAllToEnhancedInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	for (const FEazyActionBindingHandle& BindingHandle : AllActionBindings)
	{
		if (const TSharedPtr<FEazyActionBinding> Binding = FEazyActionBinding::FindBinding(BindingHandle); Binding && IsValid(Binding->InputAction.Get()))
		{
			EnsureUniqueBinding(Binding->InputAction.Get(), Binding->TriggerEvent, EnhancedInputComponent);
		}
	}
}

void UEazyUIActionRouterBase::UnbindAllFromEnhancedInputComponent()
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

void UEazyUIActionRouterBase::HandlePlayerControllerChanged(APlayerController* /*NewController*/)
{
	UniqueEnhancedInputHandles.Empty();

	if (UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent())
	{
		BindAllToEnhancedInputComponent(EnhancedInputComponent);
	}

	RefreshActiveBindingsCache();
}

void UEazyUIActionRouterBase::HandleLeafmostActiveNodeChangedInternal()
{
	// Replicate what UCommonUIActionRouterBase::HandleLeafmostActiveNodeChanged does.
	OnBoundActionsUpdated().Broadcast();

	// Eagerly refresh active bindings so dispatch only performs action/event filtering.
	RefreshActiveBindingsCache();
}

void UEazyUIActionRouterBase::ProcessEnhancedInputAction(const FInputActionInstance& ActionInstance)
{
	const UInputAction* InputAction = ActionInstance.GetSourceAction().Get();
	const ETriggerEvent TriggerEvent = ActionInstance.GetTriggerEvent();
	const TPair<TObjectKey<const UInputAction>, ETriggerEvent> ActionEventKey(TObjectKey<const UInputAction>(InputAction), TriggerEvent);

	if (InputAction == nullptr)
	{
		return;
	}

	const TArray<FEazyActionBindingHandle>* MatchingBindings = ActiveBindingsByInputAction.Find(ActionEventKey);
	if (MatchingBindings == nullptr || MatchingBindings->IsEmpty())
	{
		return;
	}

	// Work on a copy so callbacks can safely call UnregisterInputAction.
	const TArray<FEazyActionBindingHandle> BindingHandlesCopy = *MatchingBindings;
	for (const FEazyActionBindingHandle& BindingHandle : BindingHandlesCopy)
	{
		const TSharedPtr<FEazyActionBinding> Binding = FEazyActionBinding::FindBinding(BindingHandle);
		if (!Binding)
		{
			continue;
		}

		Binding->Callback.ExecuteIfBound();
	}
}

UCommonActivatableWidget* UEazyUIActionRouterBase::ResolveOwningActivatable(const UWidget& Widget) const
{
	if (UCommonActivatableWidget* AsActivatable = Cast<UCommonActivatableWidget>(const_cast<UWidget*>(&Widget)))
	{
		return AsActivatable;
	}

	const TSharedPtr<SWidget> CachedWidget = Widget.GetCachedWidget();
	ULocalPlayer* OwningLocalPlayer = Widget.GetOwningLocalPlayer();
	return FindOwningActivatable(CachedWidget, OwningLocalPlayer);
}

void UEazyUIActionRouterBase::RefreshActiveBindingsCache()
{
	ActiveBindings.Empty();
	ActiveBindingsByInputAction.Empty();

	for (auto It = AllActionBindings.CreateIterator(); It; ++It)
	{
		const FEazyActionBindingHandle BindingHandle = *It;
		const TSharedPtr<FEazyActionBinding> Binding = FEazyActionBinding::FindBinding(BindingHandle);
		if (!Binding)
		{
			It.RemoveCurrent();
			continue;
		}

		if (!IsValid(Binding->RegisteringWidget.Get()) || !IsValid(Binding->InputAction.Get()))
		{
			FEazyActionBinding::RemoveBinding(BindingHandle);
			It.RemoveCurrent();
			continue;
		}

		if (!IsBindingActive(*Binding))
		{
			continue;
		}

		ActiveBindings.Add(BindingHandle);

		const TPair<TObjectKey<const UInputAction>, ETriggerEvent> ActionEventKey(TObjectKey<const UInputAction>(Binding->InputAction.Get()), Binding->TriggerEvent);
		ActiveBindingsByInputAction.FindOrAdd(ActionEventKey).Add(BindingHandle);
	}
}

bool UEazyUIActionRouterBase::IsBindingActive(const FEazyActionBinding& Binding) const
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
