#pragma once

#include "RareUserWidget.h"
#include "RareEnhancedActionDescriptorWidget.generated.h"

class URareEnhancedActionWidget;
class UStackBox;
class UDynamicEntryBox;
class URichTextBlock;
class UEnhancedInputLocalPlayerSubsystem;
class UCommonInputSubsystem;
class UInputAction;

UCLASS(ClassGroup=UI, meta=(DisableNativeTick))
class RAREUI_API URareEnhancedActionDescriptorWidget : public URareUserWidget
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR
	
	virtual void SetDisplayText(const FText& InDisplayText);
	virtual void SetRepresentedAction(UInputAction* InInput);
	virtual void SetRepresentedActions(UInputAction* InInput, UInputAction* InGamepadOverrideInputAction);

protected:
	
	/** Begin UUserWidget */
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	/** End UUserWidget */

	virtual void RebuildActionWidgets();

	void AddActionWidget(UInputAction* InInputAction, const int32 InKeyIndex = 0) const;

protected: // Set by Editor

	UPROPERTY(EditDefaultsOnly, Category = "Actions Details")
	TSubclassOf<URareEnhancedActionWidget> ActionWidget_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Actions Details")
	FVector2D ActionIconSize = FVector2D(32.0f, 32.0f);
	
protected: // Widget Bindings
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStackBox> Actions_StackBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> DisplayName_Label;

private: // Internal

	UFUNCTION()
	void HandleControlMappingRebuilt();

	TWeakObjectPtr<UInputAction> InputActionWeak;
	TWeakObjectPtr<UInputAction> GamepadOverrideInputActionWeak;

	TWeakObjectPtr<UCommonInputSubsystem> CommonInputSubsystemWeak;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputSubsystemWeak;
};
