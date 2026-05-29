#include "Device/EazyInputPreprocessor.h"
#include "Input/Events.h"
#include "Device/EazyGamepadValuesHandler.h"

bool FEazyInputPreprocessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	const FKey Key = InAnalogInputEvent.GetKey();
	
	// We're only interested in the right or left stick values.
	if (Key != EKeys::Gamepad_RightX && Key != EKeys::Gamepad_RightY &&
		Key != EKeys::Gamepad_LeftX &&  Key != EKeys::Gamepad_LeftY)
	{
		return false;
	}
	
	const float Value = InAnalogInputEvent.GetAnalogValue();
	float StickX = 0.f;
	float StickY = 0.f;

	if (Key == EKeys::Gamepad_RightX || Key == EKeys::Gamepad_LeftX)
	{
		StickX = Value;
	}

	if (Key == EKeys::Gamepad_RightY || Key == EKeys::Gamepad_LeftY)
	{
		StickY = Value;
	}

	for (auto HandlerIt = GamepadValuesHandlers.CreateIterator(); HandlerIt; ++HandlerIt)
	{
		const TWeakInterfacePtr<IEazyGamepadValuesHandler>& Handler = *HandlerIt;
		if (!Handler.IsValid())
		{
			// Remove stale listeners eagerly to keep the array compact.
			HandlerIt.RemoveCurrent();
			continue;
		}

		Handler->NativeAnalogueValuesUpdated(InAnalogInputEvent.GetKey(), StickX, StickY);
	}
	
	// We don't want to block input for other processors, we're only interested in reading values.
	return false;
}

void FEazyInputPreprocessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}
