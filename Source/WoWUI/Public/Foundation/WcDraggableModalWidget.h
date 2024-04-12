#pragma once

#include "WcUserWidget.h"
#include "WcDraggableModalWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcDraggableModalWidget : public UWcUserWidget
{
	GENERATED_BODY()

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
};
