// Copyright (C) Grip Studios. All Rights Reserved

#include "RareGameUIPolicy.h"

#include "CommonGameViewportClient.h"
#include "CommonUITypes.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "RareGameUIManagerSubsystem.h"
#include "RarePrimaryGameLayout.h"
#include "RareUISettings.h"
#include "Engine/Engine.h"

URareGameUIPolicy* URareGameUIPolicy::GetGameUIPolicy(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (URareGameUIManagerSubsystem* UIManager = UGameInstance::GetSubsystem<URareGameUIManagerSubsystem>(GameInstance))
			{
				return UIManager->GetCurrentUIPolicy();
			}
		}
	}

	return nullptr;
}

URareGameUIManagerSubsystem* URareGameUIPolicy::GetOwningUIManager() const
{
	return CastChecked<URareGameUIManagerSubsystem>(GetOuter());
}

UWorld* URareGameUIPolicy::GetWorld() const
{
	return GetOwningUIManager()->GetGameInstance()->GetWorld();
}

URarePrimaryGameLayout* URareGameUIPolicy::GetRootLayout(const ULocalPlayer* LocalPlayer) const
{
	const FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	return LayoutInfo ? LayoutInfo->RootLayout : nullptr;
}

void URareGameUIPolicy::RequestPrimaryControl(URarePrimaryGameLayout* Layout)
{
	if (LocalMultiplayerInteractionMode == ELocalMultiplayerInteractionMode::SingleToggle && Layout->IsDormant())
	{
		for (const FRootViewportLayoutInfo& LayoutInfo : RootViewportLayouts)
		{
			URarePrimaryGameLayout* RootLayout = LayoutInfo.RootLayout;
			if (IsValid(RootLayout) && !RootLayout->IsDormant())
			{
				RootLayout->SetIsDormant(true);
				break;
			}
		}
		Layout->SetIsDormant(false);
	}
}

void URareGameUIPolicy::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
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

void URareGameUIPolicy::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = false;

		if (LocalMultiplayerInteractionMode == ELocalMultiplayerInteractionMode::SingleToggle && !LocalPlayer->IsPrimaryPlayer())
		{
			URarePrimaryGameLayout* RootLayout = LayoutInfo->RootLayout;
			if (RootLayout && !RootLayout->IsDormant())
			{
				// We're removing a secondary player's root while it's in control - transfer control back to the primary player's root
				RootLayout->SetIsDormant(true);
				for (const FRootViewportLayoutInfo& RootLayoutInfo : RootViewportLayouts)
				{
					if (RootLayoutInfo.LocalPlayer->IsPrimaryPlayer())
					{
						if (URarePrimaryGameLayout* PrimaryRootLayout = RootLayoutInfo.RootLayout)
						{
							PrimaryRootLayout->SetIsDormant(false);
						}
					}
				}
			}
		}
	}
}

void URareGameUIPolicy::NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer)
{
	NotifyPlayerRemoved(LocalPlayer);
	const int32 LayoutInfoIdx = RootViewportLayouts.IndexOfByKey(LocalPlayer);
	if (LayoutInfoIdx != INDEX_NONE)
	{
		URarePrimaryGameLayout* Layout = RootViewportLayouts[LayoutInfoIdx].RootLayout;
		RootViewportLayouts.RemoveAt(LayoutInfoIdx);

		RemoveLayoutFromViewport(LocalPlayer, Layout);

		OnRootLayoutReleased(LocalPlayer, Layout);
	}
}

TSubclassOf<URareActionKeyWidget> URareGameUIPolicy::GetActionKeyWidget()
{
	return DefaultEnhancedActionWidgetClass;
}

void URareGameUIPolicy::AddLayoutToViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout)
{
	UE_LOG(LogRareUI, Log, TEXT("[%s::%hs] -> Adding player [%s]'s root layout [%s] to the viewport"), *StaticClass()->GetName(), __func__,
	       *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

	ensureAlwaysMsgf(LocalPlayer->ViewportClient->IsA(UCommonGameViewportClient::StaticClass()),
	                 TEXT(
		                 "GameViewportClient is not of class UCommonGameViewportClient, go to project settings and set correct Viewport Class"
	                 ));

	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));

	const URareUISettings* UISettings = GetDefault<URareUISettings>();
	if (ensureAlwaysMsgf(IsValid(UISettings), TEXT("UI Settings not found, defaultZOrder passed as default value")))
	{
		Layout->AddToPlayerScreen(UISettings->ViewportLayoutDefaultZOrder);
	}
	else
	{
		Layout->AddToPlayerScreen(10'000);
	}

	OnRootLayoutAddedToViewport(LocalPlayer, Layout);
}

void URareGameUIPolicy::RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout)
{
	const TWeakPtr<SWidget> LayoutSlateWidget = Layout->GetCachedWidget();
	if (LayoutSlateWidget.IsValid())
	{
		UE_LOG(LogRareUI, Log, TEXT("[%s::%hs] -> Removing player [%s]'s root layout [%s] from the viewport"),
			   *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

		Layout->RemoveFromParent();
		if (LayoutSlateWidget.IsValid())
		{
			UE_LOG(LogRareUI, Log,
				   TEXT(
					   "[%s::%hs] -> Player [%s]'s root layout [%s] has been removed from the viewport, but other references to its underlying Slate widget still exist. Noting in case we leak it."
				   ), *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));
		}

		OnRootLayoutRemovedFromViewport(LocalPlayer, Layout);
	}
}

TObjectPtr<UInputMappingContext> URareGameUIPolicy::GetInputMapping() const
{
	return GenericInputMapping;
}

void URareGameUIPolicy::OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout)
{
#if WITH_EDITOR
	if (GIsEditor && LocalPlayer->IsPrimaryPlayer())
	{
		// So our controller will work in PIE without needing to click in the viewport
		FSlateApplication::Get().SetUserFocusToGameViewport(0);
	}
#endif

	if (CommonUI::IsEnhancedInputSupportEnabled() && GenericInputMapping)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(GenericInputMapping, GenericInputMappingPriority);
		}
	}
}

void URareGameUIPolicy::OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout)
{
	if (CommonUI::IsEnhancedInputSupportEnabled() && GenericInputMapping)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->RemoveMappingContext(GenericInputMapping);
		}
	}
}

void URareGameUIPolicy::OnRootLayoutReleased(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout)
{
	// Empty definition
}

void URareGameUIPolicy::CreateLayoutWidget(ULocalPlayer* LocalPlayer)
{
	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
	{
		const TSubclassOf<URarePrimaryGameLayout> LayoutWidgetClass = GetLayoutWidgetClass(LocalPlayer);
		if (ensure(LayoutWidgetClass && !LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			URarePrimaryGameLayout* NewLayoutObject = CreateWidget<URarePrimaryGameLayout>(
				PlayerController, LayoutWidgetClass);
			RootViewportLayouts.Emplace(LocalPlayer, NewLayoutObject, true);

			AddLayoutToViewport(LocalPlayer, NewLayoutObject);
		}
	}
}

TSubclassOf<URarePrimaryGameLayout> URareGameUIPolicy::GetLayoutWidgetClass(ULocalPlayer* LocalPlayer)
{
	ensureAlwaysMsgf(!LayoutClass.GetAssetName().IsEmpty(),
	                 TEXT("No layout class was specified in RareUIPolicy"));

	UAssetManager::Get().GetStreamableManager().RequestSyncLoad(LayoutClass.ToSoftObjectPath());
	check(LayoutClass.Get());

	return LayoutClass.Get();
}

TSoftClassPtr<URareGameDialog> URareGameUIPolicy::GetBaseDialogClass() const
{
	return BaseDialog_Class;
}
