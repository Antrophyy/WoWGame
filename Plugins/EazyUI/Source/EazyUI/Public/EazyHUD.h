#pragma once

#include "GameFramework/HUD.h"
#include "EazyHUD.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FHUDVisibilityChangedDelegate, const bool bIsShown, const APlayerController* OwningPlayerController);

UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API AEazyHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void PostInitializeComponents() override;

	virtual void ShowHUD() override;

	FHUDVisibilityChangedDelegate& OnHUDVisibilityChanged() { return HUDVisibilityChangedDelegate; }

private:

	FHUDVisibilityChangedDelegate HUDVisibilityChangedDelegate;
};
