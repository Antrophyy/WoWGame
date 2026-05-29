#pragma once

#include "EnhancedActionkeyMapping.h"
#include "EnhancedPlayerInput.h"

static const TArray<FEnhancedActionKeyMapping>& GetEnhancedActionKeyMappingsOf(UEnhancedPlayerInput* Input)
{
	struct EnhancedPlayerInputAccess : public UEnhancedPlayerInput
	{
		using UEnhancedPlayerInput::GetEnhancedActionMappings;
	};
	
	return static_cast<EnhancedPlayerInputAccess*>(Input)->GetEnhancedActionMappings();
}
