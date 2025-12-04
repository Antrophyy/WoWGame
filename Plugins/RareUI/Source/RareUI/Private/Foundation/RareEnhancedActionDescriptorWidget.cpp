#include "Foundation/RareEnhancedActionDescriptorWidget.h"

#include "CommonInputSubsystem.h"
#include "CommonUITypes.h"
#include "EnhancedInputSubsystems.h"
#include "ShaderCore.h"
#include "Components/DynamicEntryBox.h"
#include "Components/SizeBox.h"
#include "Components/StackBox.h"
#include "Components/RichTextBlock.h"
#include "Foundation/RareEnhancedActionWidget.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif // WITH_EDITOR

#if WITH_EDITOR
void URareEnhancedActionDescriptorWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!IsValid(ActionWidget_Class))
	{
		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has no ActionWidget_Class selected in widget defaults.")), FText::FromString(GetName())));
	}
}
#endif // WITH_EDITOR

void URareEnhancedActionDescriptorWidget::SetDisplayText(const FText& InDisplayText)
{
	DisplayName_Label->SetText(InDisplayText);
}

void URareEnhancedActionDescriptorWidget::SetRepresentedAction(UInputAction* InInput)
{
	InputActionWeak = InInput;

	RebuildActionWidgets();
}

void URareEnhancedActionDescriptorWidget::SetRepresentedActions(UInputAction* InInput, UInputAction* InGamepadOverrideInputAction)
{
	InputActionWeak = InInput;
	GamepadOverrideInputActionWeak = InGamepadOverrideInputAction;

	RebuildActionWidgets();
}

void URareEnhancedActionDescriptorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnhancedInputSubsystemWeak = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer());
	if (EnhancedInputSubsystemWeak.IsValid())
	{
		EnhancedInputSubsystemWeak->ControlMappingsRebuiltDelegate.AddUniqueDynamic(this, &ThisClass::HandleControlMappingRebuilt);
	}

	CommonInputSubsystemWeak = ULocalPlayer::GetSubsystem<UCommonInputSubsystem>(GetOwningLocalPlayer());
}

void URareEnhancedActionDescriptorWidget::NativeDestruct()
{
	if (EnhancedInputSubsystemWeak.IsValid())
	{
		EnhancedInputSubsystemWeak->ControlMappingsRebuiltDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void URareEnhancedActionDescriptorWidget::RebuildActionWidgets()
{
	TArray<FKey> ActionKeys;
	UInputAction* DesiredInputAction = nullptr;

	// Retrieve all available keys for the currently desired input action
	if (EnhancedInputSubsystemWeak.IsValid() && CommonInputSubsystemWeak.IsValid())
	{
		ECommonInputType InputType = CommonInputSubsystemWeak->GetCurrentInputType();
		if (InputType == ECommonInputType::Gamepad && GamepadOverrideInputActionWeak.IsValid())
		{
			DesiredInputAction = GamepadOverrideInputActionWeak.Get();
		}
		else
		{
			DesiredInputAction = InputActionWeak.Get();
		}

		CommonUI::GetEnhancedInputActionKeys(GetOwningLocalPlayer(), DesiredInputAction, ActionKeys);

		// TODO: Replace predicate for CommonUI function IsKeyValidForInputType after it comes in from main
		ActionKeys = ActionKeys.FilterByPredicate([InputType](const FKey& Key)
		{
			if (!Key.IsValid())
			{
				return false;
			}

			if (Key.IsTouch() && InputType == ECommonInputType::Touch)
			{
				return true;
			}
			if (Key.IsGamepadKey() && InputType == ECommonInputType::Gamepad)
			{
				return true;
			}
			if (!Key.IsTouch() && !Key.IsGamepadKey() && InputType == ECommonInputType::MouseAndKeyboard)
			{
				return true;
			}

			return false;
		});
	}

	if (ActionKeys.IsEmpty() || !DesiredInputAction)
		return;

	Actions_StackBox->ClearChildren();
	for (int32 i = 0; i < ActionKeys.Num(); i++)
	{
		AddActionWidget(DesiredInputAction, i);
	}
}

void URareEnhancedActionDescriptorWidget::AddActionWidget(UInputAction* InInputAction, const int32 InKeyIndex) const
{
	if (URareEnhancedActionWidget* ActionWidget = WidgetTree->ConstructWidget<URareEnhancedActionWidget>(ActionWidget_Class))
	{
		USizeBox* Action_SizeBox = NewObject<USizeBox>();
		Action_SizeBox->SetWidthOverride(ActionIconSize.X);
		Action_SizeBox->SetHeightOverride(ActionIconSize.Y);
		Action_SizeBox->SetContent(ActionWidget);
		
		ActionWidget->SetEnhancedInputAction(InInputAction, InKeyIndex);
		Actions_StackBox->AddChildToStackBox(Action_SizeBox);
	}
}

void URareEnhancedActionDescriptorWidget::HandleControlMappingRebuilt()
{
	RebuildActionWidgets();
}
