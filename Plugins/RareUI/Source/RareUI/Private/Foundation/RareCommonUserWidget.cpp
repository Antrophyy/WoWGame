
#include "Foundation/RareCommonUserWidget.h"

#include "Routing/RareUIActionRouterBase.h"
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

void URareCommonUserWidget::NativeDestruct()
{
	RemoveAllInputActionBindings();
	Super::NativeDestruct();
}

FRareActionBindingHandle URareCommonUserWidget::RegisterInputActionBinding(const FRareInputActionBindingArgs& BindActionArgs)
{
	if (URareUIActionRouterBase* Router = URareUIActionRouterBase::Get(*this))
	{
		const FRareActionBindingHandle Handle = Router->RegisterInputAction(*this, BindActionArgs);
		if (Handle.IsValid())
		{
			BoundInputActions.Add(Handle);
		}
		return Handle;
	}

	return FRareActionBindingHandle();
}

void URareCommonUserWidget::RemoveInputActionBinding(const FRareActionBindingHandle ActionBinding)
{
	const int32 BindingIndex = BoundInputActions.IndexOfByKey(ActionBinding);
	if (BindingIndex == INDEX_NONE)
	{
		return;
	}

	BoundInputActions[BindingIndex].Unregister();

	BoundInputActions.RemoveAtSwap(BindingIndex);
}

void URareCommonUserWidget::RemoveAllInputActionBindings()
{
	for (FRareActionBindingHandle& Handle : BoundInputActions)
	{
		Handle.Unregister();
	}

	BoundInputActions.Empty();
}

AHUD* URareCommonUserWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}
