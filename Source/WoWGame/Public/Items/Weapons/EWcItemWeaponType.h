#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

UENUM(BlueprintType)
enum class EWcItemWeaponType : uint8
{
	OneHandAxe,
	TwoHandAxe,
	Bow,
	Crossbow,
	Dagger,
	FishingPole,
	FistWeapon,
	Gun,
	OneHandMace,
	TwoHandMace,
	Polearm,
	Stave,
	OneHandSword,
	TwoHandSword,
	ThrownWeapon,
	Wand
};