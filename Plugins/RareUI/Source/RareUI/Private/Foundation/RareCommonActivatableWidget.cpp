// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareCommonActivatableWidget.h"
#include "RareClassValidation.h"

bool URareCommonActivatableWidget::TryHandleBackAction()
{
	return NativeOnHandleBackAction();
}

TOptional<FUIInputConfig> URareCommonActivatableWidget::GetDesiredInputConfig() const
{
	if (bShouldAffectInputMode)
	{
		return FUIInputConfig(InputConfig, GameMouseCaptureMode);
	}
	
	return TOptional<FUIInputConfig>();
}

#if WITH_EDITOR
void URareCommonActivatableWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	URareClassValidation::ValidateCompiledDefaultsForClass(this, CompileLog);
}
#endif // WITH_EDITOR

void URareCommonActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	NativeResetWidgetAnimationsState();
}

void URareCommonActivatableWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

AHUD* URareCommonActivatableWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}
