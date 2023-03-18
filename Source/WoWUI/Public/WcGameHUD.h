#pragma once

#include "ExtensionHUD.h"
#include "WcGameHUD.generated.h"

class UWcHUDWidget;

UCLASS(ClassGroup=UI)
class WOWUI_API AWcGameHUD final : public AExtensionHUD
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UWcHUDWidget> HUDWidgetClass;
};
