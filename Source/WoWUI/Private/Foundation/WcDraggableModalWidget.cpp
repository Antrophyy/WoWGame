#include "Foundation/WcDraggableModalWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HUD/Modals/WcDragWindowOperation.h"

FReply UWcDraggableModalWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UWcDraggableModalWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
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
