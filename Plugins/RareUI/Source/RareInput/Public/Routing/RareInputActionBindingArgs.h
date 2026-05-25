#pragma once

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Core/RareUIActionData.h"

/**
 * Container for the information needed to register a RareUI Enhanced Input action binding.
 * Similar in spirit to CommonUI's action binding argument structs.
 */
struct RAREINPUT_API FRareInputActionBindingArgs
{
	FRareInputActionBindingArgs() = default;

	FRareInputActionBindingArgs(const FRareUIActionData& InData, const ETriggerEvent InTriggerEvent, FSimpleDelegate InCallback)
		: Data(InData)
		, TriggerEvent(InTriggerEvent)
		, Callback(MoveTemp(InCallback))
	{
	}

	/** The input action to bind. */
	FRareUIActionData Data;

	/** Which trigger phase should invoke the callback. */
	ETriggerEvent TriggerEvent = ETriggerEvent::None;

	/** Callback executed when the binding fires. */
	FSimpleDelegate Callback;
};



