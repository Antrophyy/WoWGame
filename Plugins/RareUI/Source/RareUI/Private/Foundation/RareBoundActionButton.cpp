#include "Foundation/RareBoundActionButton.h"

#include "CommonTextBlock.h"
#include "Foundation/RareEnhancedActionWidget.h"
#include "Input/UIActionBinding.h"
#include "Routing/RareActionBinding.h"
#include "Routing/RareActionBindingHandle.h"

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

void URareBoundActionButton::SetRepresentedInputAction(const FRareActionBindingHandle InBindingHandle) const
{
	const TSharedPtr<FRareActionBinding> Binding = FRareActionBinding::FindBinding(InBindingHandle);
	if (Binding->DisplayName.IsEmpty())
	{
		Text_ActionName->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Text_ActionName->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	InputActionWidget->SetEnhancedInputAction(Binding->InputAction.Get());
	Text_ActionName->SetText(Binding->DisplayName);
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
