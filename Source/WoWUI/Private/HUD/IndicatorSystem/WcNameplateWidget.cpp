#include "HUD/IndicatorSystem/WcNameplateWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Foundation/PalleteWidgets/WcTextBlock.h"

void UWcNameplateWidget::NativeOnIndicatorWidgetInitialized()
{
	Super::NativeOnIndicatorWidgetInitialized();

	if (Name_Label)
	{
		Name_Label->SetText(FText::FromString(ActorToAttach->GetActorLabel()));
	}

	if (Health_ProgressBar)
	{
		Health_ProgressBar->SetPercent(0.5f);
	}
}

void UWcNameplateWidget::NativeResetWidgetToDefault()
{
	Super::NativeResetWidgetToDefault();

	if (LeftTargetArrow_Image)
	{
		LeftTargetArrow_Image->SetVisibility(ESlateVisibility::Hidden);
	}

	if (RightTargetArrow_Image)
	{
		RightTargetArrow_Image->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UWcNameplateWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LeftTargetArrow_Image)
	{
		LeftTargetArrow_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (RightTargetArrow_Image)
	{
		RightTargetArrow_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
