#include "Foundation/WLabeledButton.h"
#include "CommonTextBlock.h"

void UWLabeledButton::SetButtonText(const FText& InText)
{
	if (!ButtonLabel_Text || InText.EqualTo(ButtonLabel_Text->GetText()))
	{
		return;
	}

	ButtonText = InText;
	ButtonLabel_Text->SetText(InText);
}

void UWLabeledButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetButtonText(ButtonText.IsEmpty() ? FText::FromString(TEXT("{BUTTON_TEXT}")) : ButtonText);
    
	if (ButtonLabel_Text)
	{
		ButtonLabel_Text->SetVisibility(bShowButtonLabel ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}
