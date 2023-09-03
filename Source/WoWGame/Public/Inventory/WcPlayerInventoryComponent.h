#pragma once

#include "WcInventoryDetailsData.h"
#include "Components/ActorComponent.h"
#include "Items/WcItemDetail.h"
#include "WcPlayerInventoryComponent.generated.h"

class UDataTable;
class UWcInventoryDetailsData;
class UWcItemBase;

UCLASS()
class WOWGAME_API UWcPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static ThisClass* Get(const APawn* PlayerPawn);

	TArray<FWcItemDetail> GetPlayerItems() const { return PlayerItems; }

	uint32 GetPlayerInventoryCapacity() const;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWcInventoryDetailsData> InventoryDetailsData_Class;

private:

	TArray<FWcItemDetail> PlayerItems;

	uint32 InventoryCapacity = 0;
};
