#pragma once

#include "Foundation/WActivatableWidget.h"
#include "Engine/DataTable.h"
#include "WHUDScreen.generated.h"

class UWQuestLogWidget;
class UWUserWidget;
class UWInventoryWidget;
class UWcTargetingBehaviorComponent;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWHUDScreen : public UWActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly, meta=(Validate))
	TSoftClassPtr<UWActivatableWidget> PauseScreen_Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Actions, meta=(RowType="/Script/CommonUI.CommonInputActionDataBase", Validate))
	FDataTableRowHandle InvokePauseMenuActionData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Actions, meta=(RowType="/Script/CommonUI.CommonInputActionDataBase", Validate))
	FDataTableRowHandle TogglePlayerInventoryActionData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Actions, meta=(RowType="/Script/CommonUI.CommonInputActionDataBase", Validate))
	FDataTableRowHandle ToggleQuestLogActionData;

private: // Actions

	void HandleInvokePauseMenuAction();
	void HandleTogglePlayerInventoryAction();
	void HandleToggleQuestLogAction();

private: // Window Toggling

	void CollapseWindow(UWUserWidget* WindowToCollapse);
	void ShowWindow(UWUserWidget* WindowToShow);

private: // Internal Properties

	TWeakObjectPtr<UWcTargetingBehaviorComponent> TargetingBehaviorComponent;

	UPROPERTY()
	TArray<TObjectPtr<UWUserWidget>> OpenedWindows;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWInventoryWidget> PlayerInventory_Widget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWQuestLogWidget> QuestLog_Widget;
};
