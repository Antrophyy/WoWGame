#include "HUD/UnitFrames/WcUnitFrameWidget_Base.h"
#include "LogWoWUI.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"

void UWcUnitFrameWidget_Base::NativeConstruct()
{
	Super::NativeConstruct();

	TargetingBehaviorComponent = GetOwningPlayerPawn()->FindComponentByClass<UWcTargetingBehaviorComponent>();
	if (!TargetingBehaviorComponent.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not find Targetting Behavior Component. Clicking unit frames will not select a target."), *StaticClass()->GetName(), __func__);
	}
}

FReply UWcUnitFrameWidget_Base::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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
