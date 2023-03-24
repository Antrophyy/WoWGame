#include "HUD/IndicatorSystem/WcNameplateWidget.h"
#include "GameNativeTags.h"
#include "LogWoWUI.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
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

	TargetingBehaviorComponent = GetOwningPlayerPawn()->FindComponentByClass<UWcTargetingBehaviorComponent>();
	if (!TargetingBehaviorComponent.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not find targeting behavior component on the owning player pawn. Target selection will not work correctly."), *StaticClass()->GetName(), __func__);
	}
	else
	{
		if (TargetingBehaviorComponent->GetCurrentTarget() == ActorToAttach)
		{
			MakePrimaryTarget(true);
		}
	}

	if (!TargetSelectedListener.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.RegisterListener<FTargetSelectedMessage>(TAG_GAME_EVENT_TARGET_SELECTED, this, &ThisClass::HandleTargetSelectionChanged);
	}
}

void UWcNameplateWidget::NativeResetWidgetToDefault()
{
	Super::NativeResetWidgetToDefault();

	MakePrimaryTarget(false);

	TargetSelectedListener.Unregister();
}

FReply UWcNameplateWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (TargetingBehaviorComponent.IsValid())
	{
		if (ActorToAttach != TargetingBehaviorComponent->GetCurrentTarget())
		{
			TargetingBehaviorComponent->ChangeTarget(ActorToAttach.Get());
		}
	}

	return FReply::Handled();
}

void UWcNameplateWidget::MakePrimaryTarget(const bool bPrimaryTarget) const
{
	if (LeftTargetArrow_Image)
	{
		LeftTargetArrow_Image->SetVisibility(bPrimaryTarget ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}

	if (RightTargetArrow_Image)
	{
		RightTargetArrow_Image->SetVisibility(bPrimaryTarget ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UWcNameplateWidget::HandleTargetSelectionChanged(const FGameplayTag GameplayTag, const FTargetSelectedMessage& Message)
{
	if (Message.PreviousTarget == ActorToAttach)
	{
		MakePrimaryTarget(false);
	}
	else if (Message.CurrentTarget == ActorToAttach)
	{
		MakePrimaryTarget(true);
	}
}
