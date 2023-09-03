#pragma once

#include "Foundation/WcActivatableWidget.h"
#include "WcHUDWidget.generated.h"

class UWcUserWidget;
class UWcInventoryWidget;
class UWcTargetingBehaviorComponent;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcHUDWidget final : public UWcActivatableWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UWcActivatableWidget> PauseScreenClass;

private: // Actions

	void RegisterUIActionBindings();

	void HandleEscapeAction();

	void HandleToggleInventoryAction();

	FUIActionBindingHandle EscapeActionHandle;

	FUIActionBindingHandle ToggleInventoryActionHandle;

private: // Window Toggling

	void CollapseWindow(UWcUserWidget* WindowToCollapse);

	void ShowWindow(UWcUserWidget* WindowToShow);
	
private: // Internal Properties

	TWeakObjectPtr<UWcTargetingBehaviorComponent> TargetingBehaviorComponent;

	UPROPERTY()
	TArray<TObjectPtr<UWcUserWidget>> OpenedWindows;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcInventoryWidget> Inventory_Widget;
};
