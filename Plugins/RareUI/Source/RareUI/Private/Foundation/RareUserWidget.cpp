
#include "Foundation/RareUserWidget.h"
#include "RareClassValidation.h"

#if WITH_EDITOR
void URareUserWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	URareClassValidation::ValidateCompiledDefaultsForClass(this, CompileLog);
}
#endif // WITH_EDITOR

void URareUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NativeResetWidgetAnimationsState();
}

void URareUserWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

AHUD* URareUserWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}

