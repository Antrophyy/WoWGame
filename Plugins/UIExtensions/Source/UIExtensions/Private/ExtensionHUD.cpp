#include "ExtensionHUD.h"
#include "GameUIManagerSubsystem.h"

void AExtensionHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UGameUIManagerSubsystem>())
		{
			UIManager->NotifyPlayerHUDInitialized(this);
		}
	}
}

void AExtensionHUD::ShowHUD()
{
	Super::ShowHUD();

	HUDVisibilityChangedDelegate.Broadcast(bShowHUD, GetOwningPlayerController());
}
