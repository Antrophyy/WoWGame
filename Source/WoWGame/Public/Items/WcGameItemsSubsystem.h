#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WcGameItemsSubsystem.generated.h"

struct FWcItemDetail;
class UDataTable;
class UWcItemBase;

UCLASS(Config=Game)
class WOWGAME_API UWcGameItemsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	

	FWcItemDetail* GetItemDataById(const uint64 ItemId) const;

protected:

	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> ItemsData_Class;

	UPROPERTY()
	TObjectPtr<UDataTable> ItemsData;
};
