#include "Foundation/RareCommonUserWidget.h"
#include "RareClassValidation.h"

#if WITH_EDITOR
void URareCommonUserWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	URareClassValidation::ValidateCompiledDefaultsForClass(this, CompileLog);
}
#endif // WITH_EDITOR

void URareCommonUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NativeResetWidgetAnimationsState();
}

void URareCommonUserWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

AHUD* URareCommonUserWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}
