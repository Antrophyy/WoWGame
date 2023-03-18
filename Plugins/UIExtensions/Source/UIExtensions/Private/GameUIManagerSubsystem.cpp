#include "GameUIManagerSubsystem.h"
#include "LogUIExtensions.h"
#include "PrimaryGameLayout.h"
#include "ExtensionHUD.h"
#include "Kismet/GameplayStatics.h"

void UGameUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		LocalPlayerRemovedHandle = GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::NotifyPlayerRemoved);
		LocalPlayerAddedHandle = GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::NotifyPlayerAdded);
	}
}

void UGameUIManagerSubsystem::Deinitialize()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (LocalPlayerRemovedHandle.IsValid())
		{
			GameInstance->OnLocalPlayerRemovedEvent.Remove(LocalPlayerRemovedHandle);
		}

		if (LocalPlayerAddedHandle.IsValid())
		{
			GameInstance->OnLocalPlayerAddedEvent.Remove(LocalPlayerAddedHandle);
		}
	}

	if (PlayerHUD.IsValid())
	{
		if (HUDVisibilityChangedHandle.IsValid())
		{
			PlayerHUD->OnHUDVisibilityChanged().Remove(HUDVisibilityChangedHandle);
		}
	}

	Super::Deinitialize();
}

bool UGameUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UGameUIManagerSubsystem::AddLayoutToViewportForPlayer(APlayerController* PlayerController)
{
	if (!PlayerController) return;

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;

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

void UGameUIManagerSubsystem::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
	if (!LocalPlayer) return;

	AddLayoutToViewportForPlayer(LocalPlayer->GetPlayerController(GetWorld()));
}

void UGameUIManagerSubsystem::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (!LocalPlayer) return;

	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = false;
	}
}

void UGameUIManagerSubsystem::NotifyPlayerHUDInitialized(AExtensionHUD* HUD)
{
	if (!HUD) return;

	PlayerHUD = HUD;

	HUDVisibilityChangedHandle = PlayerHUD->OnHUDVisibilityChanged().AddUObject(this, &UGameUIManagerSubsystem::HandleHUDVisibilityChanged);
}

UPrimaryGameLayout* UGameUIManagerSubsystem::GetRootLayout(const ULocalPlayer* LocalPlayer) const
{
	const FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	return LayoutInfo ? LayoutInfo->RootLayout : nullptr;
}

void UGameUIManagerSubsystem::AddLayoutToViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	UE_LOG(LogUIExtensions, Log, TEXT("[%s::%hs] -> Adding player [%s]'s root layout [%s] to the viewport"), *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));
	Layout->AddToPlayerScreen(ViewportLayoutDefaultZOrder);

	OnRootLayoutAddedToViewport(LocalPlayer, Layout);
}

void UGameUIManagerSubsystem::RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	const TWeakPtr<SWidget> LayoutSlateWidget = Layout->GetCachedWidget();
	if (LayoutSlateWidget.IsValid())
	{
		UE_LOG(LogUIExtensions, Log, TEXT("[%s::%hs] -> Removing player [%s]'s root layout [%s] from the viewport"), *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

		Layout->RemoveFromParent();
		if (LayoutSlateWidget.IsValid())
		{
			UE_LOG(LogUIExtensions, Log, TEXT("[%s::%hs] -> Player [%s]'s root layout [%s] has been removed from the viewport, but other references to its underlying Slate widget still exist. Noting in case we leak it."), *StaticClass()->GetName(), __func__, *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));
		}

		OnRootLayoutRemovedFromViewport(LocalPlayer, Layout);
	}
}

void UGameUIManagerSubsystem::OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
#if WITH_EDITOR
	if (GIsEditor && LocalPlayer->IsPrimaryPlayer())
	{
		// So our controller will work in PIE without needing to click in the viewport
		FSlateApplication::Get().SetUserFocusToGameViewport(0);
	}
#endif
}

void UGameUIManagerSubsystem::CreateLayoutWidget(ULocalPlayer* LocalPlayer)
{
	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
	{
		const TSubclassOf<UPrimaryGameLayout> LayoutWidgetClass = GetLayoutWidgetClass(LocalPlayer);
		if (ensure(LayoutWidgetClass && !LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UPrimaryGameLayout* NewLayoutObject = CreateWidget<UPrimaryGameLayout>(PlayerController, LayoutWidgetClass);
			RootViewportLayouts.Emplace(LocalPlayer, NewLayoutObject, true);

			AddLayoutToViewport(LocalPlayer, NewLayoutObject);
		}
	}
}

TSubclassOf<UPrimaryGameLayout> UGameUIManagerSubsystem::GetLayoutWidgetClass(ULocalPlayer* LocalPlayer) const
{
	return LayoutClass.LoadSynchronous();
}
