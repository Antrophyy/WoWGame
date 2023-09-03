#pragma once

#include "UObject/Object.h"
#include "WcInventoryDetailsData.generated.h"

// This struct holds all the inventory details about the player's inventory. Can be configured from within the editor by the designer.
UCLASS(Abstract, Blueprintable, BlueprintType)
class WOWGAME_API UWcInventoryDetailsData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	uint32 InventoryCapacity = 0;
};
