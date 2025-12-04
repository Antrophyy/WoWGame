#pragma once

#include "Foundation/WUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "WIndicatorLayerWidget.generated.h"

class UGameBaseIndicatorWidget;
class UWIndicatorHandler_Nameplate;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWIndicatorLayerWidget final : public UWUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly, meta=(Validate))
	TSoftClassPtr<UGameBaseIndicatorWidget> NameplateWidget_Class;

private: // Internal
	void CreateComponents();

	UPROPERTY()
	TObjectPtr<UWIndicatorHandler_Nameplate> IndicatorHandler_Nameplates;
};
