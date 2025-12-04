#pragma once

#include "RareHUD.h"
#include "WGameHUD.generated.h"

class UWItemTooltipWidget;
class UCommonActivatableWidget;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API AWGameHUD final : public ARareHUD
{
	GENERATED_BODY()

public:
	UWItemTooltipWidget* GetCachedTooltipWidget() const { return CachedItemTooltip_Widget; }
	
protected:
	virtual void BeginPlay() override;

protected: // Set by the Editor
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> HUDScreen_Class;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWItemTooltipWidget> ItemTooltip_Class;
	
private: // Internal

	UPROPERTY()
	TObjectPtr<UWItemTooltipWidget> CachedItemTooltip_Widget;
};
