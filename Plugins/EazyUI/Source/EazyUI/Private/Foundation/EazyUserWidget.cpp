#include "Foundation/EazyUserWidget.h"
#include "EazyClassValidation.h"

#if WITH_EDITOR
void UEazyUserWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	UEazyClassValidation::ValidateCompiledDefaultsForClass(this, CompileLog);
}
#endif // WITH_EDITOR

void UEazyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeViewModelsForWidget(this, ViewModels);

	NativeResetWidgetAnimationsState();
}

void UEazyUserWidget::NativeDestruct()
{
	ReleaseViewModelsForWidget(this, ViewModels);

	Super::NativeDestruct();
}

void UEazyUserWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

AHUD* UEazyUserWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}
