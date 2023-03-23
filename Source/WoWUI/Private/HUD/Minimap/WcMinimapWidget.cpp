#include "HUD/Minimap/WcMinimapWidget.h"
#include "LogWoWUI.h"
#include "Characters/WcGameCharacter.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"

void UWcMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameCharacter = GetOwningPlayerPawn<AWcGameCharacter>();
	if (!GameCharacter.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not get an owning player pawn. Minimap will not work."), *StaticClass()->GetName(), __func__);
		return;
	}

	SceneCaptureComponent = GameCharacter->FindComponentByClass<USceneCaptureComponent2D>();
	if (!GameCharacter.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not get SceneCaptureComponent. Minimap will not work."), *StaticClass()->GetName(), __func__);
		return;
	}
}

void UWcMinimapWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GameCharacter.IsValid() || !PlayerArrow_Image)
		return;

	PlayerArrow_Image->SetRenderTransformAngle(GameCharacter->GetActorRotation().Yaw);
}

FReply UWcMinimapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!GameCharacter.IsValid() || !SceneCaptureComponent.IsValid() || !bZoomingAllowed)
		return FReply::Handled();

	if (InMouseEvent.GetWheelDelta() < 0)
	{
		// MWheel Down
		const float DesiredMinimapZoom = SceneCaptureComponent->OrthoWidth + ZoomStep;

		if (DesiredMinimapZoom <= MaxZoom)
		{
			SceneCaptureComponent->OrthoWidth = DesiredMinimapZoom;
		}
	}
	else
	{
		// MWheel Up
		const float DesiredMinimapZoom = SceneCaptureComponent->OrthoWidth - ZoomStep;

		if (DesiredMinimapZoom >= MinZoom)
		{
			SceneCaptureComponent->OrthoWidth = DesiredMinimapZoom;
		}
	}

	return FReply::Handled();
}
