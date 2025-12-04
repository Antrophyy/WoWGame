#include "Game/HUD/UnitFrames/WUnitFrameWidget_Base.h"
#include "LogWoWUI.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "GameFramework/Pawn.h"

void UWUnitFrameWidget_Base::NativeConstruct()
{
	Super::NativeConstruct();

	TargetingBehaviorComponent = GetOwningPlayerPawn()->FindComponentByClass<UWcTargetingBehaviorComponent>();
	if (!TargetingBehaviorComponent.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not find Targetting Behavior Component. Clicking unit frames will not select a target."), *StaticClass()->GetName(), __func__);
	}
}

FReply UWUnitFrameWidget_Base::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (TargetingBehaviorComponent.IsValid() && Owner.IsValid())
	{
		if (Owner != TargetingBehaviorComponent->GetCurrentTarget())
		{
			TargetingBehaviorComponent->ChangeTarget(Owner.Get());
		}
	}

	return FReply::Handled();
}
