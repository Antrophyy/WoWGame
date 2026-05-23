// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareBoundActionButton.h"

#include "CommonTextBlock.h"
#include "CommonUITypes.h"
#include "Foundation/RareEnhancedActionWidget.h"
#include "Input/UIActionBinding.h"

void URareBoundActionButton::SetDisplayText(const FText& InDisplayText)
{
	if (InDisplayText.EqualTo(Text_ActionName->GetText()))
	{
		return;
	}

	bCustomDisplayText = true;
	CustomDisplayText = InDisplayText;
	Text_ActionName->SetText(CustomDisplayText);
}

void URareBoundActionButton::SetRepresentedAction(const FUIActionBindingHandle InBindingHandle)
{
	Super::SetRepresentedAction(InBindingHandle);

	if (InBindingHandle.GetDisplayName().IsEmpty())
	{
		Text_ActionName->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Text_ActionName->SetVisibility(ESlateVisibility::Visible);
	}

	if (const TSharedPtr<FUIActionBinding> Binding = FUIActionBinding::FindBinding(InBindingHandle))
	{
		const FCommonInputActionDataBase* DataTableRow = Binding->GetLegacyInputActionData();
		bRequiresHold = DataTableRow->HasHoldBindings();
		UpdateInputActionWidget();
		if (const UCommonInputSubsystem* InputSubsystem = GetInputSubsystem())
		{
			HoldTime = DataTableRow->GetCurrentInputTypeInfo(InputSubsystem).HoldTime;
			HoldRollbackTime = DataTableRow->GetCurrentInputTypeInfo(InputSubsystem).HoldRollbackTime;
		}
	}
}

void URareBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	RareEnhancedActionWidget = Cast<URareEnhancedActionWidget>(InputActionWidget);
}

void URareBoundActionButton::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	if (bCustomDisplayText)
	{
		Text_ActionName->SetText(CustomDisplayText);
	}
	
	if (RareEnhancedActionWidget && bRequiresHold)
	{
		// We want to remove the (HOLD) from the text for hold actions.
		Text_ActionName->SetText(RareEnhancedActionWidget->GetActionDisplayText());
	}
}
