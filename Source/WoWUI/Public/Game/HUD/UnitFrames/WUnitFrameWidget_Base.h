#pragma once

#include "Blueprint/UserWidget.h"
#include "WUnitFrameWidget_Base.generated.h"

class UTextBlock;
class AActor;
class UCommonNumericTextBlock;
class UImage;
class UProgressBar;
class UWcTargetingBehaviorComponent;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWUnitFrameWidget_Base : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:

	TWeakObjectPtr<AActor> Owner;

	TWeakObjectPtr<UWcTargetingBehaviorComponent> TargetingBehaviorComponent;

protected: // Widget Bindings

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> UnitName_Label;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCommonNumericTextBlock> UnitLevel_NumericLabel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Portrait_Image;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Health_ProgressBar;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Resource_ProgressBar;
};
