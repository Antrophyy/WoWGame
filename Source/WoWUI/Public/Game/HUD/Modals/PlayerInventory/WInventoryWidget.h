#pragma once

#include "Foundation/WDraggableModalWidget.h"
#include "WInventoryWidget.generated.h"

class UWItemTooltipWidget;
class UWItemWidget;
class UUniformGridPanel;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWInventoryWidget : public UWDraggableModalWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected: // Set by the Editor

	UPROPERTY(EditDefaultsOnly, meta=(Validate))
	TSubclassOf<UWItemWidget> InventoryItem_Class;

	UPROPERTY(EditDefaultsOnly, meta=(Validate))
	TSubclassOf<UWItemTooltipWidget> ItemTooltip_Widget;

private: // Internal
	
	void FetchPlayerInventoryItems();
	
private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid_Panel;
};
