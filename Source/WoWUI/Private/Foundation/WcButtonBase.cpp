#include "Foundation/WcButtonBase.h"
#include "CommonTextBlock.h"

void UWcButtonBase::SetButtonText(const FText& InText)
{
	if (InText.ToString() == ButtonText.ToString() || !ButtonText_Label)
		return;

	ButtonText = InText;

	ButtonText_Label->SetText(InText);
}

void UWcButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (ButtonText_Label)
	{
		ButtonText_Label->SetText(bShowButtonLabel ? ButtonText : FText::GetEmpty());
		ButtonText_Label->SetVisibility(bShowButtonLabel ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}
