#include "Game/HUD/UnitFrames/WUnitFrameWidget_Player.h"

void UWUnitFrameWidget_Player::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = GetOwningPlayerPawn();
}
