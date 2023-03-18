#pragma once

#include "GameFramework/HUD.h"
#include "ExtensionHUD.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FHUDVisibilityChangedDelegate, const bool bIsShown, const APlayerController* OwningPlayerController);

UCLASS(ClassGroup=UI)
class UIEXTENSIONS_API AExtensionHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void PostInitializeComponents() override;

	virtual void ShowHUD() override;

	FHUDVisibilityChangedDelegate& OnHUDVisibilityChanged() { return HUDVisibilityChangedDelegate; }

private:

	FHUDVisibilityChangedDelegate HUDVisibilityChangedDelegate;
};
