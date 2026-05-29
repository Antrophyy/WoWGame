#include "EazyGameUIManagerSubsystem.h"

#include "CommonGameViewportClient.h"
#include "EazyGameUIPolicy.h"
#include "EazyPrimaryGameLayout.h"
#include "EazyHUD.h"
#include "EazyUISettings.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Editor.h"
#endif // WITH_EDITOR

void UEazyGameUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	const UEazyUISettings* EazyUISettings = GetDefault<UEazyUISettings>();
	if (!ensureAlwaysMsgf(!EazyUISettings->DefaultUIPolicy.IsNull(), TEXT("DefaultUIPolicy is not set in EazyUISettings. Please set it to a valid UEazyGameUIPolicy Blueprint class.")))
	{
		return;
	}
	
	if (!IsValid(CurrentPolicy))
	{
		DefaultUIPolicyClass = EazyUISettings->DefaultUIPolicy.LoadSynchronous();
		
		UEazyGameUIPolicy* CreatedPolicy = NewObject<UEazyGameUIPolicy>(this, DefaultUIPolicyClass);
		SwitchToPolicy(CreatedPolicy);
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		LocalPlayerRemovedHandle = GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::NotifyLocalPlayerRemoved);
		LocalPlayerAddedHandle = GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::NotifyLocalPlayerAdded);
	}

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick), 0.0f);

	if (EazyUISettings->bEnableAddToViewportEnsures)
	{
		if (UGameViewportSubsystem* GameViewportSubsystem = UGameViewportSubsystem::Get())
		{
			GameViewportSubsystem->OnWidgetAdded.AddUObject(this, &ThisClass::OnWidgetAddedToViewport);
		}
	}
}

void UEazyGameUIManagerSubsystem::Deinitialize()
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

	SwitchToPolicy(nullptr);

	FTSTicker::RemoveTicker(TickHandle);

	const UEazyUISettings* EazyUISettings = GetDefault<UEazyUISettings>();

	if (EazyUISettings->bEnableAddToViewportEnsures)
	{
		if (UGameViewportSubsystem* GameViewportSubsystem = UGameViewportSubsystem::Get())
		{
			GameViewportSubsystem->OnWidgetAdded.RemoveAll(this);
		}
	}
		
	Super::Deinitialize();
}

bool UEazyGameUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() > 0;
	}

#if WITH_EDITOR
	if (GEditor && GEditor->IsSimulateInEditorInProgress())
	{
		return false;
	}
#endif// WITH_EDITOR

	return true;
}

void UEazyGameUIManagerSubsystem::NotifyLocalPlayerAdded(ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
	{
		return;
	}

	NotifyPlayerAdded(InLocalPlayer->PlayerController);

	if (!IsValid(InLocalPlayer->PlayerController))
	{
		InLocalPlayer->OnPlayerControllerChanged().RemoveAll(this);
		
		// Too bad, we can't do anything until the player controller is set, so let's wait.
		InLocalPlayer->OnPlayerControllerChanged().AddUObject(this, &ThisClass::NotifyPlayerAdded);
	}
}

void UEazyGameUIManagerSubsystem::NotifyLocalPlayerRemoved(ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
	{
		return;
	}

	NotifyPlayerRemoved(InLocalPlayer->PlayerController);
	
	InLocalPlayer->OnPlayerControllerChanged().RemoveAll(this);
}

void UEazyGameUIManagerSubsystem::SwitchToPolicy(UEazyGameUIPolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}

bool UEazyGameUIManagerSubsystem::Tick(const float)
{
	SyncRootLayoutVisibilityToShowHUD();
	return true;
}

void UEazyGameUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{
	for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
	{
		bool bShouldShowUI = true;

		const APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
		if (IsValid(PlayerController))
		{
			const AHUD* HUD = PlayerController->GetHUD();

			if (IsValid(HUD) && !HUD->bShowHUD)
			{
				bShouldShowUI = false;
			}
		}

		SyncHudVisibilityForPlayer(bShouldShowUI, PlayerController);
	}
}

void UEazyGameUIManagerSubsystem::OnWidgetAddedToViewport(UWidget* InAddedWidget, ULocalPlayer* InLocalPlayer) const
{
	const UEazyUISettings* EazyUISettings = GetDefault<UEazyUISettings>();
	
	if (!IsValid(InAddedWidget))
	{
		return;
	}

	// Check for allowed types
	for (const TObjectPtr<UClass>& AllowedClassType : EazyUISettings->ClassTypesToIgnoreForAddToViewport)
	{
		if (InAddedWidget->IsA(AllowedClassType))
		{
			// We're allowed.
			return;
		}
	}
	
	/**
	 * Zero tolerance policy
	 * Reason: If you need UI added to the viewport, request it from the UI via task. We are not using AddToViewport ever as we have layers to work with. Using this would lead to undefined behaviour.
	 * 
	 * Use debug UI (ImGui) or prints if you need to debug something.
	 *
	 * If you did not cause this, remember the name of InAddedWidget and potentially callstack and tell UI.
	 */
	ensureMsgf(false, TEXT("A widget %s has been added to a viewport via AddToViewport. This should not happen on this project, contact UI programmer."), *InAddedWidget->GetName());
}

void UEazyGameUIManagerSubsystem::NotifyPlayerAdded(APlayerController* PlayerController)
{
	if (PlayerController && IsValid(CurrentPolicy))
	{
		CurrentPolicy->NotifyPlayerAdded(PlayerController->GetLocalPlayer());
	}
}

void UEazyGameUIManagerSubsystem::NotifyPlayerRemoved(APlayerController* PlayerController)
{
	if (PlayerController && IsValid(CurrentPolicy))
	{
		CurrentPolicy->NotifyPlayerRemoved(PlayerController->GetLocalPlayer());
	}
}

void UEazyGameUIManagerSubsystem::NotifyPlayerHUDInitialized(AEazyHUD* HUD)
{
	if (!IsValid(HUD))
	{
		return;
	}

	PlayerHUD = HUD;
}

void UEazyGameUIManagerSubsystem::SyncHudVisibilityForPlayer(const bool bIsShown, const APlayerController* OwningPlayerController)
{
	if (!IsValid(GetCurrentUIPolicy()) || !IsValid(OwningPlayerController))
	{
		return;
	}
		
	if (UEazyPrimaryGameLayout* RootLayout = GetCurrentUIPolicy()->GetRootLayout(OwningPlayerController->GetLocalPlayer()))
	{
		const ESlateVisibility DesiredVisibility = bIsShown ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
		if (DesiredVisibility != RootLayout->GetVisibility())
		{
			RootLayout->SetVisibility(DesiredVisibility);
		}
	}
}
