#pragma once

#include "UObject/Object.h"
#include "WcIndicatorComponent_Base.generated.h"

class APlayerController;
class UBaseIndicatorWidget;

UCLASS(Abstract)
class WOWUI_API UWcIndicatorComponent_Base : public UObject
{
	GENERATED_BODY()

public:

	void InitializeComponent(APlayerController* InOwningPlayer, const TSoftClassPtr<UBaseIndicatorWidget> InIndicatorWidgetClass);

protected:

	virtual void NativeComponentInitialized() {}

	TWeakObjectPtr<APlayerController> OwningPlayer;

	TSoftClassPtr<UBaseIndicatorWidget> IndicatorWidgetClass;
};
