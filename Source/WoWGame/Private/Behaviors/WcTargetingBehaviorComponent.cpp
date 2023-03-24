#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "GameNativeTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"

void UWcTargetingBehaviorComponent::ChangeTarget(AActor* NewTarget)
{
	if (NewTarget == CurrentTarget)
		return;

	PreviousTarget = CurrentTarget;
	CurrentTarget = NewTarget;
	
	BroadcastTargetSelected();
}

void UWcTargetingBehaviorComponent::BroadcastTargetSelected() const
{
	FTargetSelectedMessage Message;
	Message.CurrentTarget = CurrentTarget;
	Message.PreviousTarget = PreviousTarget;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(TAG_GAME_EVENT_TARGET_SELECTED, Message);
}
