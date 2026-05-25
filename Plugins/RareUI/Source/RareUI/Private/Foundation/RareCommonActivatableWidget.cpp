#include "Foundation/RareCommonActivatableWidget.h"

#include "LogRareUI.h"
#include "Routing/RareUIActionRouterBase.h"
#include "RareClassValidation.h"
#include "RareGameUIPolicy.h"

bool URareCommonActivatableWidget::TryHandleBackAction()
{
	return NativeOnHandleBackAction();
}

TOptional<FUIInputConfig> URareCommonActivatableWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(InputConfig, GameMouseCaptureMode);
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

void URareCommonActivatableWidget::NativeConstruct()
{
	// We don't want to call CommonUI's NativeConstruct as it adds RegisterUIActionBinding stuff that we don't want with our input implementation.
	UUserWidget::NativeConstruct();
	
	if (bIsBackHandler)
	{
		if (URareGameUIPolicy* UIPolicy = URareGameUIPolicy::GetGameUIPolicy(this))
		{
			FRareUIActionData BackActionData(UIPolicy->GetBackInputActionData());
			if (!OverrideBackActionDisplayName.IsEmpty())
			{
				BackActionData.DisplayName = OverrideBackActionDisplayName;
			}
			
			BackActionData.bDisplayInActionBar = bIsBackActionDisplayedInActionBar;
			
			RegisterInputActionBinding(FRareInputActionBindingArgs(BackActionData, ETriggerEvent::Triggered, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
		}
	}

	if (bAutoActivate)
	{
		UE_LOGF(LogRareUI, Verbose, "[%ls] auto-activated", *GetName());
		ActivateWidget();
	}
}

void URareCommonActivatableWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

void URareCommonActivatableWidget::NativeDestruct()
{
	RemoveAllInputActionBindings();
	Super::NativeDestruct();
}

FRareActionBindingHandle URareCommonActivatableWidget::RegisterInputActionBinding(const FRareInputActionBindingArgs& BindActionArgs)
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

void URareCommonActivatableWidget::RemoveInputActionBinding(FRareActionBindingHandle ActionBinding)
{
	const int32 BindingIndex = BoundInputActions.IndexOfByKey(ActionBinding);
	if (BindingIndex == INDEX_NONE)
	{
		return;
	}

	BoundInputActions[BindingIndex].Unregister();

	BoundInputActions.RemoveAtSwap(BindingIndex);
}

void URareCommonActivatableWidget::RemoveAllInputActionBindings()
{
	for (FRareActionBindingHandle& Handle : BoundInputActions)
	{
		Handle.Unregister();
	}

	BoundInputActions.Empty();
}

AHUD* URareCommonActivatableWidget::GetHUD() const
{
	return IsValid(GetOwningPlayer()) ? GetOwningPlayer()->GetHUD() : nullptr;
}
