#include "Game/HUD/IndicatorSystem/WNameplateWidget.h"
#include "GameNativeTags.h"
#include "LogWoWUI.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"

void UWNameplateWidget::NativeOnIndicatorWidgetInitialized()
{
	Super::NativeOnIndicatorWidgetInitialized();

	ActorToAttach = Cast<AActor>(DataObject);
	Name_Label->SetText(FText::FromString(ActorToAttach->GetName()));
	Health_ProgressBar->SetPercent(0.5f);

	TargetingBehaviorComponent = GetOwningPlayerPawn()->FindComponentByClass<UWcTargetingBehaviorComponent>();
	if (!TargetingBehaviorComponent.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("Could not find targeting behavior component on the owning player pawn. Target selection will not work correctly."));
		return;
	}

	if (TargetingBehaviorComponent->GetCurrentTarget() == DataObject)
	{
		MakePrimaryTarget(true);
	}

	if (!TargetSelectedListener.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.RegisterListener<FTargetSelectedMessage>(TAG_GAME_EVENT_TARGET_SELECTED, this, &ThisClass::HandleTargetSelectionChanged);
	}
}

void UWNameplateWidget::NativeResetWidgetToDefault()
{
	Super::NativeResetWidgetToDefault();

	TargetSelectedListener.Unregister();
}

FReply UWNameplateWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (TargetingBehaviorComponent.IsValid())
	{
		if (ActorToAttach != TargetingBehaviorComponent->GetCurrentTarget())
		{
			TargetingBehaviorComponent->ChangeTarget(ActorToAttach.Get());
		}
	}

	return FReply::Unhandled();
}

void UWNameplateWidget::MakePrimaryTarget(const bool bPrimaryTarget) const
{
	const ESlateVisibility DesiredVisibility = bPrimaryTarget ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
	
	LeftTargetArrow_Image->SetVisibility(DesiredVisibility);
	RightTargetArrow_Image->SetVisibility(DesiredVisibility);
}

void UWNameplateWidget::HandleTargetSelectionChanged(const FGameplayTag, const FTargetSelectedMessage& Message)
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
