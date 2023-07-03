#include "Foundation/ExtensionActivatableWidget.h"
#include "LogUIExtensions.h"
#include "Framework/Application/SlateUser.h"

TOptional<FUIInputConfig> UExtensionActivatableWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(InputConfig, GameMouseCaptureMode);
}

void UExtensionActivatableWidget::Refocus() const
{
	if (UWidget* FocusTarget = GetDesiredFocusTarget())
	{
		UE_LOG(LogUIExtensions, Display, TEXT("[User %d] Refocused desired target %s."), GetOwnerSlateUser()->GetUserIndex(), *FocusTarget->GetName());
		FocusTarget->SetUserFocus(GetOwningPlayer());
	}
}