#pragma once

#include "Blueprint/UserWidget.h"
#include "WcUnitFrameWidget_Base.generated.h"

class AActor;
class UCommonNumericTextBlock;
class UImage;
class UProgressBar;
class UWcTextBlock;
class UWcTargetingBehaviorComponent;

UCLASS(Abstract, NotBlueprintable, NotBlueprintType)
class WOWUI_API UWcUnitFrameWidget_Base : public UUserWidget
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
	TObjectPtr<UWcTextBlock> UnitName_Label;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCommonNumericTextBlock> UnitLevel_NLabel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Portrait_Image;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Health_ProgressBar;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Resource_ProgressBar;
};
