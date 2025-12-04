#include "Game/HUD/Minimap/WMinimapWidget.h"
#include "LogWoWUI.h"
#include "Characters/WcGameCharacter.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextBlock.h"

void UWMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameCharacterWeak = GetOwningPlayerPawn<AWcGameCharacter>();
	if (!GameCharacterWeak.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not get an owning player pawn. Minimap will not work."), *StaticClass()->GetName(), __func__);
		return;
	}

	SceneCaptureComponentWeak = GameCharacterWeak->FindComponentByClass<USceneCaptureComponent2D>();
	if (!GameCharacterWeak.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not get SceneCaptureComponent. Minimap will not work."), *StaticClass()->GetName(), __func__);
		return;
	}
}

void UWMinimapWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameCharacterWeak.IsValid())
	{
		PlayerArrow_Image->SetRenderTransformAngle(GameCharacterWeak->GetActorRotation().Yaw);
	}

	WorldTime_Label->SetText(FText::FromString(FDateTime::Now().ToFormattedString(TEXT("%H:%M %p"))));
}

FReply UWMinimapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!GameCharacterWeak.IsValid() || !SceneCaptureComponentWeak.IsValid() || !bZoomingAllowed)
	{
		return FReply::Handled();
	}

	if (InMouseEvent.GetWheelDelta() < 0)
	{
		// MWheel Down
		const float DesiredMinimapZoom = SceneCaptureComponentWeak->OrthoWidth + ZoomStep;

		if (DesiredMinimapZoom <= MaxZoom)
		{
			SceneCaptureComponentWeak->OrthoWidth = DesiredMinimapZoom;
		}
	}
	else
	{
		// MWheel Up
		const float DesiredMinimapZoom = SceneCaptureComponentWeak->OrthoWidth - ZoomStep;

		if (DesiredMinimapZoom >= MinZoom)
		{
			SceneCaptureComponentWeak->OrthoWidth = DesiredMinimapZoom;
		}
	}

	return FReply::Handled();
}
