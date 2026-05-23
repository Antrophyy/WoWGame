// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Input/CommonAnalogCursor.h"

class RAREUI_API FRareAnalogCursor final : public FCommonAnalogCursor
{
public:
	explicit FRareAnalogCursor(const UCommonUIActionRouterBase& InActionRouter)
		: FCommonAnalogCursor(InActionRouter)
	{	}

	void SetAnalogMovementEnabled(bool bEnable);
};
