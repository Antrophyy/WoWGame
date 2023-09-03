#include "HUD/Modals/Inventory/WcInventoryWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/WcPlayerInventoryComponent.h"
#include "HUD/WcItemWidget.h"
#include "Components/UniformGridPanel.h"
#include "HUD/Modals/WcDragWindowOperation.h"

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
	const UWcPlayerInventoryComponent* InventoryComponent = UWcPlayerInventoryComponent::Get(GetOwningPlayerPawn());
	if (!IsValid(InventoryComponent))
		return;

	for (FWcItemDetail ItemDetails : InventoryComponent->GetPlayerItems())
	{
		// TODO(Jan.Vlcek): Here we will fetch all items the player has and put them to a grid. Placeholder for now.
		for (int Row = 0; Row < 6; ++Row)
		{
			for (int Column = 0; Column < 6; ++Column)
			{
				UWcItemWidget* InventoryItem_Widget = CreateWidget<UWcItemWidget>(this, InventoryItem_Class);
				InventoryItem_Widget->SetItemDetails(ItemDetails);
				InventoryItem_Widget->SetItemIcon(ItemDetails.Thumbnail.LoadSynchronous());
				InventoryGrid_Panel->AddChildToUniformGrid(InventoryItem_Widget, Row, Column);
			}
		}
	}
}
