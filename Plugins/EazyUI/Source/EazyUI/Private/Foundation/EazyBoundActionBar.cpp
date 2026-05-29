#include "Foundation/EazyBoundActionBar.h"

#include "CommonButtonBase.h"
#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "CommonUITypes.h"
#include "Editor/WidgetCompilerLog.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateUser.h"
#include "InputAction.h"
#include "Foundation/EazyBoundActionButton.h"
#include "Input/CommonBoundActionButtonInterface.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/UIActionBinding.h"
#include "Layout/WidgetPath.h"
#include "Routing/EazyUIActionRouterBase.h"

#define LOCTEXT_NAMESPACE "EazyUI"

void UEazyBoundActionBar::SetDisplayOwningPlayerActionsOnly(bool bShouldOnlyDisplayOwningPlayerActions)
{
	if (bShouldOnlyDisplayOwningPlayerActions != bDisplayOwningPlayerActionsOnly)
	{
		bDisplayOwningPlayerActionsOnly = bShouldOnlyDisplayOwningPlayerActions;
		if (!IsDesignTime())
		{
			HandleBoundActionsUpdated(true);
		}
	}
}

void UEazyBoundActionBar::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (GameInstance->GetGameViewportClient())
		{
			GameInstance->GetGameViewportClient()->OnPlayerAdded().RemoveAll(this);
			GameInstance->GetGameViewportClient()->OnPlayerRemoved().RemoveAll(this);

			GameInstance->GetGameViewportClient()->OnPlayerAdded().AddUObject(this, &ThisClass::HandlePlayerAdded);
			GameInstance->GetGameViewportClient()->OnPlayerRemoved().AddUObject(this, &ThisClass::HandlePlayerRemoved);
		}

		for (const ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
		{
			MonitorPlayerActions(LocalPlayer);
		}

		// Establish entries (as needed) immediately upon construction
		HandleDeferredDisplayUpdate();
	}
}

void UEazyBoundActionBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UEazyBoundActionBar::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		for (const ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
		{
			if (const UCommonUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(LocalPlayer))
			{
				ActionRouter->OnBoundActionsUpdated().RemoveAll(this);
			}

			if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
			{
				InputSubsystem->OnInputMethodChangedNative.RemoveAll(this);
			}
		}
	}
}

#if WITH_EDITOR
void UEazyBoundActionBar::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!ActionButtonClass)
	{
		CompileLog.Error(FText::Format(LOCTEXT("Error_BoundActionBar_MissingButtonClass", "{0} has no ActionButtonClass specified."), FText::FromString(GetName())));
	}
	else if (CompileLog.GetContextClass() && ActionButtonClass->IsChildOf(CompileLog.GetContextClass()))
	{
		CompileLog.Error(FText::Format(LOCTEXT("Error_BoundActionBar_RecursiveButtonClass", "{0} has a recursive ActionButtonClass specified (reference itself)."), FText::FromString(GetName())));
	}
}
#endif

void UEazyBoundActionBar::HandleBoundActionsUpdated(const bool bFromOwningPlayer)
{
	if (bFromOwningPlayer || !bDisplayOwningPlayerActionsOnly)
	{
		UpdateDisplay();
	}
}

void UEazyBoundActionBar::HandleInputMappingsRebuiltUpdated()
{
	UpdateDisplay();
}

void UEazyBoundActionBar::UpdateDisplay()
{
	if (!bIsRefreshQueued)
	{
		bIsRefreshQueued = true;

		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float)
		{
			if (IsSafeToUpdateDisplay())
			{
				HandleDeferredDisplayUpdate();
				return false;
			}
			return true;
		}));
	}
}

bool UEazyBoundActionBar::IsSafeToUpdateDisplay() const
{
	return !DoAnyActionButtonsHaveMouseCapture();
}

void UEazyBoundActionBar::HandleDeferredDisplayUpdate()
{
	ActionBarUpdateBegin();

	bIsRefreshQueued = false;

	ResetInternal();

	const UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);
	const ULocalPlayer* OwningLocalPlayer = GetOwningLocalPlayer();

	// Sort the player list so our owner is at the end
	TArray<ULocalPlayer*> SortedPlayers = GameInstance->GetLocalPlayers();
	SortedPlayers.StableSort(
		[&OwningLocalPlayer](const ULocalPlayer& PlayerA, const ULocalPlayer&)
		{
			return &PlayerA != OwningLocalPlayer;
		});

	for (const ULocalPlayer* LocalPlayer : SortedPlayers)
	{
		if (LocalPlayer == OwningLocalPlayer || !bDisplayOwningPlayerActionsOnly)
		{
			if (IsEntryClassValid(ActionButtonClass))
			{
				if (const UEazyUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UEazyUIActionRouterBase>(LocalPlayer))
				{
					const UCommonInputSubsystem& InputSubsystem = ActionRouter->GetInputSubsystem();
					const ECommonInputType PlayerInputType = InputSubsystem.GetCurrentInputType();

					TSet<FName> AcceptedBindings;
					TArray<FEazyActionBindingHandle> FilteredBindings = ActionRouter->GetActiveBindings().FilterByPredicate([ActionRouter, PlayerInputType, &AcceptedBindings, this](const FEazyActionBindingHandle& Handle) mutable
					{
						if (const TSharedPtr<FEazyActionBinding> Binding = FEazyActionBinding::FindBinding(Handle))
						{
							if (!Binding->bDisplayInActionBar)
							{
								return false;
							}

							const bool bIsValidEnhancedInputAction = CommonUI::ActionValidForInputType(ActionRouter->GetLocalPlayer(), PlayerInputType, Binding->InputAction.Get());
							if (!bIsValidEnhancedInputAction)
							{
								return false;
							}

							if (!bIgnoreDuplicateActions)
							{
								return true;
							}

							bool bAlreadyAccepted = false;
							AcceptedBindings.Add(Binding->InputAction->GetFName(), &bAlreadyAccepted);
							return !bAlreadyAccepted;
						}

						return false;
					});

					//Force Virtual_Back to one end of the list so Back actions are always consistent.
					//Otherwise, order within a node is controlled by order of add/remove.
					Algo::Sort(FilteredBindings, [](const FEazyActionBindingHandle& A, const FEazyActionBindingHandle& B)
					{
						const TSharedPtr<FEazyActionBinding> BindingA = FEazyActionBinding::FindBinding(A);
						const TSharedPtr<FEazyActionBinding> BindingB = FEazyActionBinding::FindBinding(B);

						if (ensureMsgf(BindingA && BindingB, TEXT("The array filter above should enforce that there are no null bindings")))
						{
							auto GetNavBarPriority = [](const TSharedPtr<FEazyActionBinding>& ActionBinding)
							{
								if (ActionBinding.IsValid())
								{
									return ActionBinding->ActionBarPriority;
								}

								return 0;
							};

							const UInputAction* InputActionA = BindingA->InputAction.Get();
							const UInputAction* InputActionB = BindingB->InputAction.Get();

							if (IsValid(InputActionA) && IsValid(InputActionB))
							{
								const int32 NavBarPriorityA = GetNavBarPriority(BindingA);
								const int32 NavBarPriorityB = GetNavBarPriority(BindingB);

								if (NavBarPriorityA != NavBarPriorityB)
								{
									return NavBarPriorityA < NavBarPriorityB;
								}
							}

							return GetTypeHash(BindingA->Handle) < GetTypeHash(BindingB->Handle);
						}

						return true; // A < B by default
					});

					for (FEazyActionBindingHandle BindingHandle : FilteredBindings)
					{
						UEazyBoundActionButton* ActionButton = Cast<UEazyBoundActionButton>(CreateActionButton(BindingHandle));
						if (ensure(ActionButton))
						{
							ActionButton->SetRepresentedInputAction(BindingHandle);
							NativeOnActionButtonCreated(ActionButton, BindingHandle);
						}
					}
				}
			}
		}
	}

	ActionBarUpdateEnd();
}

UUserWidget* UEazyBoundActionBar::CreateActionButton(const FEazyActionBindingHandle& BindingHandle)
{
	return CreateEntryInternal(ActionButtonClass);
}

void UEazyBoundActionBar::HandlePlayerAdded(const int32 PlayerIdx)
{
	const ULocalPlayer* NewPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIdx);
	MonitorPlayerActions(NewPlayer);
	HandleBoundActionsUpdated(NewPlayer == GetOwningLocalPlayer());
}

void UEazyBoundActionBar::HandlePlayerRemoved(const int32 PlayerIdx)
{
	const ULocalPlayer* RemovedPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIdx);
	HandleBoundActionsUpdated(RemovedPlayer == GetOwningLocalPlayer());
}

void UEazyBoundActionBar::HandledInputTypeUpdated(ECommonInputType)
{
	UpdateDisplay();
}

void UEazyBoundActionBar::MonitorPlayerActions(const ULocalPlayer* NewPlayer)
{
	if (const UCommonUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(NewPlayer))
	{
		ActionRouter->OnBoundActionsUpdated().AddUObject(this, &ThisClass::HandleBoundActionsUpdated, NewPlayer == GetOwningLocalPlayer());
	}

	/**
	 * Update available inputs anytime the input type changes;
	 * in Enhanced Input, an input action might be only bound with one device (gamepad), but not others (KBM / mouse),
	 * due to that they will be considered invalid in case the action bar is updated when using the device that doesn't
	 * have them bound to anything. When switching to a device that actually binds them to an input, the action bar
	 * has to refresh to include the input actions that were considered invalid prior to that
	 */
	if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(NewPlayer))
	{
		InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandledInputTypeUpdated);
	}

	// need to check the owning player here rather than the in the callback because the dynamic delegates can't have extra params. So just don't subscribe if not needed
	const bool bFromOwningPlayer = NewPlayer == GetOwningLocalPlayer();
	if (bFromOwningPlayer || !bDisplayOwningPlayerActionsOnly)
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer))
		{
			EnhancedInputLocalPlayerSubsystem->ControlMappingsRebuiltDelegate.AddUniqueDynamic(this, &ThisClass::HandleInputMappingsRebuiltUpdated);
		}
	}
}

void UEazyBoundActionBar::ActionBarUpdateBegin()
{
	ActionBarUpdateBeginImpl();
}

void UEazyBoundActionBar::ActionBarUpdateEnd()
{
	ActionBarUpdateEndImpl();
}

bool UEazyBoundActionBar::DoAnyActionButtonsHaveMouseCapture() const
{
	if (const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer())
	{
		if (const TSharedPtr<const FSlateUser> SlateUser = LocalPlayer->GetSlateUser())
		{
			if (SlateUser->HasAnyCapture())
			{
				const FWeakWidgetPath CapturePath = SlateUser->GetWeakCursorCapturePath();
				for (const UUserWidget* Element : GetAllEntries())
				{
					if (CapturePath.ContainsWidget(Element->GetCachedWidget().Get()))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
