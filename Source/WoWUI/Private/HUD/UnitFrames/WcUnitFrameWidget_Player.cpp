#include "HUD/UnitFrames/WcUnitFrameWidget_Player.h"

void UWcUnitFrameWidget_Player::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = GetOwningPlayerPawn();
}
