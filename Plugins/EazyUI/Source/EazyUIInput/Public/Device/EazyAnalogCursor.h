#pragma once

#include "Input/CommonAnalogCursor.h"

class EAZYUIINPUT_API FEazyAnalogCursor final : public FCommonAnalogCursor
{
public:
	explicit FEazyAnalogCursor(const UCommonUIActionRouterBase& InActionRouter)
		: FCommonAnalogCursor(InActionRouter)
	{	}

	void SetAnalogMovementEnabled(bool bEnable);
};
