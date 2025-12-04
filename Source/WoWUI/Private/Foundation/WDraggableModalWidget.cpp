#include "Foundation/WDraggableModalWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Game/HUD/Modals/WcDragWindowOperation.h"

FReply UWDraggableModalWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UWDraggableModalWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
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
