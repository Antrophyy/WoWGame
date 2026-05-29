#pragma once

#include "EazyHUD.h"
#include "WGameHUD.generated.h"

class UWItemTooltipWidget;
class UEazyCommonActivatableWidget;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API AWGameHUD final : public AEazyHUD
{
	GENERATED_BODY()

public:
	UWItemTooltipWidget* GetCachedTooltipWidget() const { return CachedItemTooltip_Widget; }
	
protected:
	virtual void BeginPlay() override;

protected: // Set by the Editor
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UEazyCommonActivatableWidget> HUDScreen_Class;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWItemTooltipWidget> ItemTooltip_Class;
	
private: // Internal

	UPROPERTY()
	TObjectPtr<UWItemTooltipWidget> CachedItemTooltip_Widget;
};
