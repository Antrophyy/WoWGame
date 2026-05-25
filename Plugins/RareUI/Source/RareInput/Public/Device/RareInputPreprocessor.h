
#pragma once

#include "InputCoreTypes.h"
#include "Framework/Application/IInputProcessor.h"
#include "UObject/WeakInterfacePtr.h"

class IRareGamepadValuesHandler;

class RAREINPUT_API FRareInputPreprocessor final :  public IInputProcessor
{
public:
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

	TArray<TWeakInterfacePtr<IRareGamepadValuesHandler>> GamepadValuesHandlers;
};

