#pragma once

#include "Input/CommonAnalogCursor.h"

class RAREUI_API FRareAnalogCursor final : public FCommonAnalogCursor
{
public:
	explicit FRareAnalogCursor(const UCommonUIActionRouterBase& InActionRouter)
		: FCommonAnalogCursor(InActionRouter)
	{
		bShouldHandleRightAnalog = false;
	}

	void SetAnalogMovementEnabled(bool bEnable);
};
