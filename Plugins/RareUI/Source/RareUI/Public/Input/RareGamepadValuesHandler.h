// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "InputCoreTypes.h"
#include "Elements/Actor/ActorElementAssetDataInterface.h"
#include "RareGamepadValuesHandler.generated.h"

UINTERFACE(MinimalAPI, Meta=(CannotImplementInterfaceInBlueprint))
class URareGamepadValuesHandler : public UInterface
{
	GENERATED_BODY()
};

class RAREUI_API IRareGamepadValuesHandler
{
	GENERATED_BODY()
	
public:
	virtual void NativeAnalogueValuesUpdated(const FKey InKey, const float StickX, const float StickY) {};
};