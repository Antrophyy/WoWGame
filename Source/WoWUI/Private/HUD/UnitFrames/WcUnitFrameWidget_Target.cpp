#include "HUD/UnitFrames/WcUnitFrameWidget_Target.h"
#include "CommonNumericTextBlock.h"
#include "GameNativeTags.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Foundation/PalleteWidgets/WcTextBlock.h"
#include "GameFramework/GameplayMessageSubsystem.h"

void UWcUnitFrameWidget_Target::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!TargetSelectedListener.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.RegisterListener<FTargetSelectedMessage>(TAG_GAME_EVENT_TARGET_SELECTED, this, &ThisClass::HandleTargetSelectionChanged);
	}
}

void UWcUnitFrameWidget_Target::HandleTargetSelectionChanged(const FGameplayTag GameplayTag, const FTargetSelectedMessage& Message)
{
	Owner = Message.CurrentTarget;
	
	if (!Message.CurrentTarget.IsValid())
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	UnitName_Label->SetText(FText::FromString(Message.CurrentTarget->GetActorLabel()));

	// TODO: Add current level.
	UnitLevel_NLabel->SetCurrentValue(60);

	// #TODO: Add current health.
	Health_ProgressBar->SetPercent(100.f);

	// #TODO: Add current resource.
	Resource_ProgressBar->SetPercent(100.f);

	// #TODO: Add portrait image.
	// Portrait_Image->SetBrushFromTexture()

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
