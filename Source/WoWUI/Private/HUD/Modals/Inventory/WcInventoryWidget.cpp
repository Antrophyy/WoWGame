#include "HUD/Modals/Inventory/WcInventoryWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HUD/Modals/Inventory/WcInventoryItemWidget.h"
#include "Components/UniformGridPanel.h"
#include "HUD/WcItemTooltipWidget.h"
#include "HUD/Modals/WcDragWindowOperation.h"
#include "Items/EWcItemBinding.h"
#include "Items/EWcItemRarity.h"
#include "Items/Weapons/EWcItemWeaponType.h"
#include "Items/Weapons/WcWeaponItem.h"

void UWcInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FetchPlayerInventoryItems();
}

FReply UWcInventoryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UWcInventoryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UWcDragWindowOperation* DragWindowOperation = NewObject<UWcDragWindowOperation>();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	DragWindowOperation->WidgetReference = this;
	DragWindowOperation->DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	DragWindowOperation->DefaultDragVisual = this;
	DragWindowOperation->Pivot = EDragPivot::MouseDown;
	
	OutOperation = DragWindowOperation;
}

void UWcInventoryWidget::FetchPlayerInventoryItems()
{
	UWcWeaponItem* WeaponItem = NewObject<UWcWeaponItem>();
	WeaponItem->Type = EWcItemWeaponType::Crossbow;
	WeaponItem->Name = FText::FromString("Weapon of Testing");
	WeaponItem->Rarity = EWcItemRarity::Epic;
	WeaponItem->ItemBinding = EWcItemBinding::BindOnPickup;
	WeaponItem->LevelRequirement = 20;
	
	// TODO(Jan.Vlcek): Here we will fetch all items the player has and put them to a grid. Placeholder for now.
	for (int Row = 0; Row < 6; ++Row)
	{
		for (int Column = 0; Column < 6; ++Column)
		{
			UWcInventoryItemWidget* InventoryItem = CreateWidget<UWcInventoryItemWidget>(this, InventoryItem_Widget);

			UWcItemTooltipWidget* TooltipWidget = CreateWidget<UWcItemTooltipWidget>(this, ItemTooltip_Widget);
			InventoryItem->SetToolTip(TooltipWidget);
			TooltipWidget->InitializeTooltipInformation(WeaponItem);
			InventoryGrid_Panel->AddChildToUniformGrid(InventoryItem, Row, Column);
		}
	}
}
