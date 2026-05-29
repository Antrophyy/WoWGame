#pragma once

#include "InputCoreTypes.h"
#include "Framework/Application/IInputProcessor.h"
#include "UObject/WeakInterfacePtr.h"

class IEazyGamepadValuesHandler;

class EAZYUIINPUT_API FEazyInputPreprocessor final :  public IInputProcessor
{
public:
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

	TArray<TWeakInterfacePtr<IEazyGamepadValuesHandler>> GamepadValuesHandlers;
};
