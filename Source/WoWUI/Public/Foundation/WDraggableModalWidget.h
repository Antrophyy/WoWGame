#pragma once

#include "WUserWidget.h"
#include "WDraggableModalWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWDraggableModalWidget : public UWUserWidget
{
	GENERATED_BODY()

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
};
