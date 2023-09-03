#include "Items/WcGameItemsSubsystem.h"

#include "Engine/DataTable.h"
#include "Items/WcItemDetail.h"

void UWcGameItemsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ItemsData = ItemsData_Class.LoadSynchronous();
}

FWcItemDetail* UWcGameItemsSubsystem::GetItemDataById(const uint64 ItemId) const
{
	check(ItemsData);

	return ItemsData->FindRow<FWcItemDetail>(FName(FString::FromInt(ItemId)), "GET ITEM BY ID");
}
