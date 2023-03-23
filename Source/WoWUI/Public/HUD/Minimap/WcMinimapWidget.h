#pragma once

#include "Foundation/WcUserWidget.h"
#include "WcMinimapWidget.generated.h"

class AWcGameCharacter;
class UImage;
class USceneCaptureComponent2D;

UCLASS(Abstract)
class WOWUI_API UWcMinimapWidget final : public UWcUserWidget
{
	GENERATED_BODY()

protected: // Overriden Functions

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected: // Set by the Editor

	// Whether you can zoom in and out in the minimap with a scroll wheel.
	UPROPERTY(EditAnywhere)
	bool bZoomingAllowed = true;

	// By how much does one turn of mouse wheel affect the minimap zoom.
	UPROPERTY(EditAnywhere, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="bZoomingAllowed", EditConditionHides))
	float ZoomStep = 100.f;

	// What is the maximum allowed zoom of the minimap.
	UPROPERTY(EditAnywhere, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="bZoomingAllowed", EditConditionHides))
	float MaxZoom = 2500.f;

	// What is the minimum allowed zoom of the minimap.
	UPROPERTY(EditAnywhere, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="bZoomingAllowed", EditConditionHides))
	float MinZoom = 1500.f;

private:

	TWeakObjectPtr<AWcGameCharacter> GameCharacter;

	TWeakObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PlayerArrow_Image;
};
