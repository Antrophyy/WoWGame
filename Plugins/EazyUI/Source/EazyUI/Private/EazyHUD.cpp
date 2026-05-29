#include "EazyHUD.h"
#include "EazyGameUIManagerSubsystem.h"

void AEazyHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UEazyGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UEazyGameUIManagerSubsystem>())
		{
			UIManager->NotifyPlayerHUDInitialized(this);
		}
	}
}

void AEazyHUD::ShowHUD()
{
	Super::ShowHUD();

	HUDVisibilityChangedDelegate.Broadcast(bShowHUD, GetOwningPlayerController());
}
