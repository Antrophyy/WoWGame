#pragma once

#include "Foundation/WUserWidget.h"
#include "WMinimapWidget.generated.h"

class UTextBlock;
class AWcGameCharacter;
class UImage;
class USceneCaptureComponent2D;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWMinimapWidget final : public UWUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected: // Set by the Editor

	// Whether you can zoom in and out in the minimap with a scroll wheel.
	UPROPERTY(EditDefaultsOnly, Category=Configuration)
	bool bZoomingAllowed = true;

	// By how much does one turn of mouse wheel affect the minimap zoom.
	UPROPERTY(EditDefaultsOnly, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="bZoomingAllowed", EditConditionHides), Category=Configuration)
	float ZoomStep = 100.f;

	// What is the maximum allowed zoom of the minimap.
	UPROPERTY(EditDefaultsOnly, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="bZoomingAllowed", EditConditionHides), Category=Configuration)
	float MaxZoom = 2500.f;

	// What is the minimum allowed zoom of the minimap.
	UPROPERTY(EditDefaultsOnly, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="bZoomingAllowed", EditConditionHides), Category=Configuration)
	float MinZoom = 1500.f;

private: // Internal
	
	TWeakObjectPtr<AWcGameCharacter> GameCharacterWeak;
	TWeakObjectPtr<USceneCaptureComponent2D> SceneCaptureComponentWeak;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PlayerArrow_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WorldTime_Label;
};
