#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

UENUM(BlueprintType)
enum class EWcItemRarity : uint8
{
	Poor,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Artifact,
	Heirloom
};
