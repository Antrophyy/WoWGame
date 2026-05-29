#pragma once

#include "InputCoreTypes.h"
#include "Elements/Actor/ActorElementAssetDataInterface.h"
#include "EazyGamepadValuesHandler.generated.h"

UINTERFACE(MinimalAPI, Meta=(CannotImplementInterfaceInBlueprint))
class UEazyGamepadValuesHandler : public UInterface
{
	GENERATED_BODY()
};

class EAZYUIINPUT_API IEazyGamepadValuesHandler
{
	GENERATED_BODY()
	
public:
	virtual void NativeAnalogueValuesUpdated(const FKey InKey, const float StickX, const float StickY) {}
};
