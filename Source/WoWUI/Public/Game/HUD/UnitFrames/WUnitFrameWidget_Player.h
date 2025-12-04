#pragma once

#include "WUnitFrameWidget_Base.h"
#include "WUnitFrameWidget_Player.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWUnitFrameWidget_Player : public UWUnitFrameWidget_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};
