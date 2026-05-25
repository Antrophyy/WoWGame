
#pragma once

#include "Input/CommonAnalogCursor.h"

class RAREINPUT_API FRareAnalogCursor final : public FCommonAnalogCursor
{
public:
	explicit FRareAnalogCursor(const UCommonUIActionRouterBase& InActionRouter)
		: FCommonAnalogCursor(InActionRouter)
	{	}

	void SetAnalogMovementEnabled(bool bEnable);
};


