#include "Foundation/EazyBoundActionButton.h"

#include "CommonTextBlock.h"
#include "Foundation/EazyEnhancedActionWidget.h"
#include "Input/UIActionBinding.h"
#include "Routing/EazyActionBinding.h"
#include "Routing/EazyActionBindingHandle.h"

void UEazyBoundActionButton::SetDisplayText(const FText& InDisplayText)
{
	if (InDisplayText.EqualTo(Text_ActionName->GetText()))
	{
		return;
	}

	bCustomDisplayText = true;
	CustomDisplayText = InDisplayText;
	Text_ActionName->SetText(CustomDisplayText);
}

void UEazyBoundActionButton::SetRepresentedInputAction(const FEazyActionBindingHandle InBindingHandle) const
{
	const TSharedPtr<FEazyActionBinding> Binding = FEazyActionBinding::FindBinding(InBindingHandle);
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

void UEazyBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	EazyEnhancedActionWidget = Cast<UEazyEnhancedActionWidget>(InputActionWidget);
}

void UEazyBoundActionButton::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	if (bCustomDisplayText)
	{
		Text_ActionName->SetText(CustomDisplayText);
	}
	
	if (EazyEnhancedActionWidget && bRequiresHold)
	{
		// We want to remove the (HOLD) from the text for hold actions.
		Text_ActionName->SetText(EazyEnhancedActionWidget->GetActionDisplayText());
	}
}
