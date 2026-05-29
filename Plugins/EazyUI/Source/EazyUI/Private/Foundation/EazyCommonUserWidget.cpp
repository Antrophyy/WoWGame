#include "Foundation/EazyCommonUserWidget.h"

#include "Routing/EazyUIActionRouterBase.h"
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

	NativeResetWidgetAnimationsState();
}

void UEazyCommonUserWidget::NativeResetWidgetAnimationsState()

{
	BP_ResetWidgetAnimationsState();
}

void UEazyCommonUserWidget::NativeDestruct()
{
	RemoveAllInputActionBindings();
	Super::NativeDestruct();
}

FEazyActionBindingHandle UEazyCommonUserWidget::RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs)
{
	if (UEazyUIActionRouterBase* Router = UEazyUIActionRouterBase::Get(*this))
	{
		const FEazyActionBindingHandle Handle = Router->RegisterInputAction(*this, BindActionArgs);
		if (Handle.IsValid())
		{
			BoundInputActions.Add(Handle);
		}
		return Handle;
	}

	return FEazyActionBindingHandle();
}

void UEazyCommonUserWidget::RemoveInputActionBinding(const FEazyActionBindingHandle ActionBinding)
{
	const int32 BindingIndex = BoundInputActions.IndexOfByKey(ActionBinding);
	if (BindingIndex == INDEX_NONE)
	{
		return;
	}

	BoundInputActions[BindingIndex].Unregister();

	BoundInputActions.RemoveAtSwap(BindingIndex);
}

void UEazyCommonUserWidget::RemoveAllInputActionBindings()
{
	for (FEazyActionBindingHandle& Handle : BoundInputActions)
	{
		Handle.Unregister();
	}

	BoundInputActions.Empty();
}

AHUD* UEazyCommonUserWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}
