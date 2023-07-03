#pragma once

#include "Foundation/WcUserWidget.h"
#include "WcInventoryWidget.generated.h"

class UWcItemTooltipWidget;
class UWcInventoryItemWidget;
class UUniformGridPanel;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcInventoryWidget : public UWcUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

protected: // Set by the Editor

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWcInventoryItemWidget> InventoryItem_Widget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWcItemTooltipWidget> ItemTooltip_Widget;

private:

	void FetchPlayerInventoryItems();
	
private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid_Panel;
};
