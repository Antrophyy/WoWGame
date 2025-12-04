#include "Input/RareInputPreprocessor.h"
#include "Input/Events.h"
#include "Input/RareGamepadValuesHandler.h"

bool FRareInputPreprocessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	const FKey Key = InAnalogInputEvent.GetKey();
	// We're only interested in the right or left stick values.
	if ((Key != EKeys::Gamepad_RightX && Key != EKeys::Gamepad_RightY) &&
		(Key != EKeys::Gamepad_LeftX &&  Key != EKeys::Gamepad_LeftY))
	{
		return false;
	}
	
	for (const TWeakInterfacePtr<IRareGamepadValuesHandler> Handler : GamepadValuesHandlers)
	{
		if (!Handler.IsValid())
		{
			// Probably forgot to remove listener, oh well.
			continue;
		}
		
		const float Value = InAnalogInputEvent.GetAnalogValue();
		float StickX = 0.f, StickY = 0.f;
		
		if (Key == EKeys::Gamepad_RightX || Key == EKeys::Gamepad_LeftX)
		{
			StickX = Value;
		}

		if (Key == EKeys::Gamepad_RightY || Key == EKeys::Gamepad_LeftY)
		{
			StickY = Value;
		}

		Handler->NativeAnalogueValuesUpdated(InAnalogInputEvent.GetKey(), StickX, StickY);
	}
	
	// We don't want to block input for other processors, we're only interested in reading values.
	return false;
}

void FRareInputPreprocessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}
