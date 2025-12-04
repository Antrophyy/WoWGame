#pragma once

#include "UObject/Object.h"
#include "WIndicatorHandler_Base.generated.h"

class UGameBaseIndicatorWidget;
class APlayerController;

UCLASS(Abstract)
class WOWUI_API UWIndicatorHandler_Base : public UObject
{
	GENERATED_BODY()

public:
	void InitializeComponent(APlayerController* InOwningPlayer, const TSoftClassPtr<UGameBaseIndicatorWidget> InIndicatorWidgetClass);

protected:
	virtual void NativeComponentInitialized() {}
	
	TWeakObjectPtr<APlayerController> OwningPlayer;
	TSoftClassPtr<UGameBaseIndicatorWidget> IndicatorWidgetClass;
};
