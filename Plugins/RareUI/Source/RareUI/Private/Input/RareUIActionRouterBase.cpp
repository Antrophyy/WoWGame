// Copyright (C) Grip Studios. All Rights Reserved

#include "Input/RareUIActionRouterBase.h"

#include "Input/RareAnalogCursor.h"
#include "Input/RareInputPreprocessor.h"

void URareUIActionRouterBase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AnalogCursor = StaticCastSharedPtr<FRareAnalogCursor, FCommonAnalogCursor>(GetCommonAnalogCursor());
	RareInputPreprocessor = MakeShared<FRareInputPreprocessor>();
	FSlateApplication::Get().RegisterInputPreProcessor(RareInputPreprocessor, 0);
}

void URareUIActionRouterBase::Deinitialize()
{
	if (RareInputPreprocessor.IsValid())
	{
		RareInputPreprocessor->GamepadValuesHandlers.Reset();
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(RareInputPreprocessor);
		}
		RareInputPreprocessor.Reset();
	}
	
	Super::Deinitialize();
}

void URareUIActionRouterBase::SetAnalogMovementEnabled(const bool bEnable) const
{
	AnalogCursor->SetAnalogMovementEnabled(bEnable);
}

void URareUIActionRouterBase::AddGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const
{
	if (RareInputPreprocessor.IsValid())
	{
		RareInputPreprocessor->GamepadValuesHandlers.Add(InHandler);
	}
}
void URareUIActionRouterBase::RemoveGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const
{
	if (RareInputPreprocessor.IsValid() && RareInputPreprocessor->GamepadValuesHandlers.Contains(InHandler))
	{
		RareInputPreprocessor->GamepadValuesHandlers.Remove(InHandler);
	}
}

bool URareUIActionRouterBase::CanProcessNormalGameInput() const
{
	return true;
}

TSharedRef<FCommonAnalogCursor> URareUIActionRouterBase::MakeAnalogCursor() const
{
	return FCommonAnalogCursor::CreateAnalogCursor<FRareAnalogCursor>(*this);
}
