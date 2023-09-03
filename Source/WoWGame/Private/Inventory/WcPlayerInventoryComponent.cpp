#include "Inventory/WcPlayerInventoryComponent.h"

#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Items/WcGameItemsSubsystem.h"

UWcPlayerInventoryComponent::ThisClass* UWcPlayerInventoryComponent::Get(const APawn* PlayerPawn)
{
	if (!PlayerPawn)
		return nullptr;

	return PlayerPawn->GetComponentByClass<ThisClass>();
}

uint32 UWcPlayerInventoryComponent::GetPlayerInventoryCapacity() const
{
	check(IsValid(InventoryDetailsData_Class))

	if (const UWcInventoryDetailsData* Data = InventoryDetailsData_Class->GetDefaultObject<UWcInventoryDetailsData>())
		return Data->InventoryCapacity;

	return 0;
}

void UWcPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const UWcGameItemsSubsystem* GameItemsSubsystem = UWorld::GetSubsystem<UWcGameItemsSubsystem>(GetWorld()))
	{
		PlayerItems.Add(*GameItemsSubsystem->GetItemDataById(0));
	}
}
