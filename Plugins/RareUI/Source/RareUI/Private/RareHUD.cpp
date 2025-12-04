#include "RareHUD.h"
#include "RareGameUIManagerSubsystem.h"

void ARareHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (URareGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<URareGameUIManagerSubsystem>())
		{
			UIManager->NotifyPlayerHUDInitialized(this);
		}
	}
}

void ARareHUD::ShowHUD()
{
	Super::ShowHUD();

	HUDVisibilityChangedDelegate.Broadcast(bShowHUD, GetOwningPlayerController());
}
