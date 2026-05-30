#include "Foundation/EazyCommonUserWidget.h"

#include "EazyClassValidation.h"

#if WITH_EDITOR
void UEazyCommonUserWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	UEazyClassValidation::ValidateCompiledDefaultsForClass(this, CompileLog);
}
#endif // WITH_EDITOR

void UEazyCommonUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeViewModelsForWidget(this, ViewModels);

	NativeResetWidgetAnimationsState();
}

void UEazyCommonUserWidget::NativeDestruct()
{
	ReleaseViewModelsForWidget(this, ViewModels);
	RemoveAllInputActionBindingsForWidget(BoundInputActions);
	Super::NativeDestruct();
}

void UEazyCommonUserWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

FEazyActionBindingHandle UEazyCommonUserWidget::RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs)
{
	return RegisterInputActionBindingForWidget(*this, BindActionArgs, BoundInputActions);
}

void UEazyCommonUserWidget::RemoveInputActionBinding(const FEazyActionBindingHandle ActionBinding)
{
	RemoveInputActionBindingForWidget(ActionBinding, BoundInputActions);
}
