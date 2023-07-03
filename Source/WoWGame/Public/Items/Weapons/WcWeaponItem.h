#pragma once

#include "Items/WcItemBase.h"
#include "WcWeaponItem.generated.h"

enum class EWcItemWeaponType : uint8;

UCLASS()
class WOWGAME_API UWcWeaponItem : public UWcItemBase
{
	GENERATED_BODY()

public:

	EWcItemWeaponType Type;
};
