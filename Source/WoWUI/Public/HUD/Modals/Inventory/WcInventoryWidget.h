#pragma once

#include "Foundation/WcDraggableModalWidget.h"
#include "WcInventoryWidget.generated.h"

class UWcItemTooltipWidget;
class UWcItemWidget;
class UUniformGridPanel;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcInventoryWidget : public UWcDraggableModalWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

protected: // Set by the Editor

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWcItemWidget> InventoryItem_Class;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWcItemTooltipWidget> ItemTooltip_Widget;

private:

	void FetchPlayerInventoryItems();
	
private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid_Panel;
};
