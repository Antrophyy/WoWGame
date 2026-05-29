#pragma once

#include "InputAction.h"
#include "EazyUIActionData.h"

// Container for the information needed to register a EazyUI Enhanced Input action binding.
// Similar in spirit to CommonUI's action binding argument structs.
struct EAZYUIINPUT_API FEazyInputActionBindingArgs
{
	FEazyInputActionBindingArgs() = default;

	FEazyInputActionBindingArgs(const FEazyUIActionData& InData, const ETriggerEvent InTriggerEvent, FSimpleDelegate InCallback)
		: Data(InData)
		, TriggerEvent(InTriggerEvent)
		, Callback(MoveTemp(InCallback))
	{
	}

	// The input action to bind.
	FEazyUIActionData Data;

	// Which trigger phase should invoke the callback.
	ETriggerEvent TriggerEvent = ETriggerEvent::None;

	// Callback executed when the binding fires.
	FSimpleDelegate Callback;
};
