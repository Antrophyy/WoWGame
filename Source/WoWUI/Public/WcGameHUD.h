#pragma once

#include "ExtensionHUD.h"
#include "WcGameHUD.generated.h"

class UWcItemTooltipWidget;
class UCommonActivatableWidget;

UCLASS(ClassGroup=UI)
class WOWUI_API AWcGameHUD final : public AExtensionHUD
{
	GENERATED_BODY()

public:

	UWcItemTooltipWidget* GetCachedTooltipWidget() const { return CachedItemTooltip_Widget; }

// protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> HUDWidget_Class;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWcItemTooltipWidget> ItemTooltip_Class;

private:

	UPROPERTY()
	TObjectPtr<UWcItemTooltipWidget> CachedItemTooltip_Widget;
};
