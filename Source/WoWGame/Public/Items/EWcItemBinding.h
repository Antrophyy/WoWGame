#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

UENUM(BlueprintType)
enum class EWcItemBinding : uint8
{
	BindOnPickup,
	BindOnAcquire,
	BindOnUse,
	BindToAccount
};
