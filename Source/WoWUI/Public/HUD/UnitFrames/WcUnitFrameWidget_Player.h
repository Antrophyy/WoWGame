#pragma once

#include "WcUnitFrameWidget_Base.h"
#include "WcUnitFrameWidget_Player.generated.h"

UCLASS(Abstract, Blueprintable)
class WOWUI_API UWcUnitFrameWidget_Player final : public UWcUnitFrameWidget_Base
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;
};
