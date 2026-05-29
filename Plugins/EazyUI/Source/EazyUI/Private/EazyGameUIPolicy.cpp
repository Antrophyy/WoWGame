#include "EazyGameUIPolicy.h"

#include "CommonGameViewportClient.h"
#include "CommonUITypes.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "EazyGameUIManagerSubsystem.h"
#include "EazyPrimaryGameLayout.h"
#include "EazyUISettings.h"
#include "Engine/Engine.h"

UEazyGameUIPolicy* UEazyGameUIPolicy::GetGameUIPolicy(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UEazyGameUIManagerSubsystem* UIManager = UGameInstance::GetSubsystem<UEazyGameUIManagerSubsystem>(GameInstance))
			{
				return UIManager->GetCurrentUIPolicy();
			}
		}
	}

	return nullptr;
}

UEazyGameUIManagerSubsystem* UEazyGameUIPolicy::GetOwningUIManager() const
{
	return CastChecked<UEazyGameUIManagerSubsystem>(GetOuter());
}

UWorld* UEazyGameUIPolicy::GetWorld() const
{
	return GetOwningUIManager()->GetGameInstance()->GetWorld();
}

UEazyPrimaryGameLayout* UEazyGameUIPolicy::GetRootLayout(const ULocalPlayer* LocalPlayer) const
{
	const FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	return LayoutInfo ? LayoutInfo->RootLayout : nullptr;
}

void UEazyGameUIPolicy::RequestPrimaryControl(UEazyPrimaryGameLayout* Layout)
{
	if (LocalMultiplayerInteractionMode == ELocalMultiplayerInteractionMode::SingleToggle && Layout->IsDormant())
	{
		for (const FRootViewportLayoutInfo& LayoutInfo : RootViewportLayouts)
		{
			UEazyPrimaryGameLayout* RootLayout = LayoutInfo.RootLayout;
			if (IsValid(RootLayout) && !RootLayout->IsDormant())
			{
				RootLayout->SetIsDormant(true);
				break;
			}
		}
		Layout->SetIsDormant(false);
	}
}

void UEazyGameUIPolicy::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = true;
	}
	else
	{
		CreateLayoutWidget(LocalPlayer);
	}
}

void UEazyGameUIPolicy::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = false;

		if (LocalMultiplayerInteractionMode == ELocalMultiplayerInteractionMode::SingleToggle && !LocalPlayer->IsPrimaryPlayer())
		{
			UEazyPrimaryGameLayout* RootLayout = LayoutInfo->RootLayout;
			if (RootLayout && !RootLayout->IsDormant())
			{
				// We're removing a secondary player's root while it's in control - transfer control back to the primary player's root
				RootLayout->SetIsDormant(true);
				for (const FRootViewportLayoutInfo& RootLayoutInfo : RootViewportLayouts)
				{
					if (RootLayoutInfo.LocalPlayer->IsPrimaryPlayer())
					{
						if (UEazyPrimaryGameLayout* PrimaryRootLayout = RootLayoutInfo.RootLayout)
						{
							PrimaryRootLayout->SetIsDormant(false);
						}
					}
				}
			}
		}
	}
}

void UEazyGameUIPolicy::NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer)
{
	NotifyPlayerRemoved(LocalPlayer);
	const int32 LayoutInfoIdx = RootViewportLayouts.IndexOfByKey(LocalPlayer);
	if (LayoutInfoIdx != INDEX_NONE)
	{
		UEazyPrimaryGameLayout* Layout = RootViewportLayouts[LayoutInfoIdx].RootLayout;
		RootViewportLayouts.RemoveAt(LayoutInfoIdx);

		RemoveLayoutFromViewport(LocalPlayer, Layout);

		OnRootLayoutReleased(LocalPlayer, Layout);
	}
}

TSubclassOf<UEazyActionKeyWidget> UEazyGameUIPolicy::GetActionKeyWidget()
{
	return DefaultEnhancedActionWidgetClass;
}

const FEazyUIActionData& UEazyGameUIPolicy::GetBackInputActionData()
{
	return BackActionData;
}

void UEazyGameUIPolicy::AddLayoutToViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout)
{
	UE_LOG(LogEazyUI, Log, TEXT("[%s::%hs] -> Adding player [%s]'s root layout [%s] to the viewport"), *StaticClass()->GetName(), __func__,
	       *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

	ensureAlwaysMsgf(!LocalPlayer->ViewportClient->IsA(UCommonGameViewportClient::StaticClass()),
	                 TEXT(
		                 "GameViewportClient is of class UCommonGameViewportClient, go to project settings and don't use this viewport as it uses legacy input."
	                 ));

	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));

	const UEazyUISettings* UISettings = GetDefault<UEazyUISettings>();
	if (ensureAlwaysMsgf(IsValid(UISettings), TEXT("UI Settings not found; defaultZOrder will be used as the default value")))
	{
		Layout->AddToPlayerScreen(UISettings->ViewportLayoutDefaultZOrder);
	}
	else
	{
		Layout->AddToPlayerScreen(10'000);
	}

	OnRootLayoutAddedToViewport(LocalPlayer, Layout);
}

void UEazyGameUIPolicy::RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout)
{
	const TWeakPtr<SWidget> LayoutSlateWidget = Layout->GetCachedWidget();
	if (LayoutSlateWidget.IsValid())
	{
		UE_LOG(LogEazyUI, Log, TEXT("[%s::%hs] -> Removing player [%s]'s root layout [%s] from the viewport"),
			   *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

		Layout->RemoveFromParent();
		if (LayoutSlateWidget.IsValid())
		{
			UE_LOG(LogEazyUI, Log,
				   TEXT(
							"[%s::%hs] -> Player [%s]'s root layout [%s] has been removed from the viewport, but other references to its underlying Slate widget still exist. Noting this in case we leak it."
				   ), *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));
		}

		OnRootLayoutRemovedFromViewport(LocalPlayer, Layout);
	}
}

void UEazyGameUIPolicy::OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout)
{
#if WITH_EDITOR
	if (GIsEditor && LocalPlayer->IsPrimaryPlayer())
	{
		// So our controller will work in PIE without needing to click in the viewport
		FSlateApplication::Get().SetUserFocusToGameViewport(0);
	}
#endif

	for (const UInputMappingContext* InputMappingContext : UserInterfaceInputMappingContexts)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void UEazyGameUIPolicy::OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout)
{
	for (const UInputMappingContext* InputMappingContext : UserInterfaceInputMappingContexts)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->RemoveMappingContext(InputMappingContext);
		}
	}
}

void UEazyGameUIPolicy::OnRootLayoutReleased(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout)
{
	// Empty definition
}

void UEazyGameUIPolicy::CreateLayoutWidget(ULocalPlayer* LocalPlayer)
{
	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
	{
		const TSubclassOf<UEazyPrimaryGameLayout> LayoutWidgetClass = GetLayoutWidgetClass(LocalPlayer);
		if (ensure(LayoutWidgetClass && !LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UEazyPrimaryGameLayout* NewLayoutObject = CreateWidget<UEazyPrimaryGameLayout>(
				PlayerController, LayoutWidgetClass);
			RootViewportLayouts.Emplace(LocalPlayer, NewLayoutObject, true);

			AddLayoutToViewport(LocalPlayer, NewLayoutObject);
		}
	}
}

TSubclassOf<UEazyPrimaryGameLayout> UEazyGameUIPolicy::GetLayoutWidgetClass(ULocalPlayer*) const
{
	ensureAlwaysMsgf(!LayoutClass.GetAssetName().IsEmpty(),
	                 TEXT("No layout class was specified in EazyUIPolicy"));

	UAssetManager::Get().GetStreamableManager().RequestSyncLoad(LayoutClass.ToSoftObjectPath());
	check(LayoutClass.Get());

	return LayoutClass.Get();
}

TSoftClassPtr<UEazyGameDialog> UEazyGameUIPolicy::GetBaseDialogClass() const
{
	return BaseDialog_Class;
}
