#include "Foundation/EazyCommonActivatableWidget.h"

#include "LogEazyUI.h"
#include "EazyClassValidation.h"
#include "EazyGameUIPolicy.h"

TOptional<FUIInputConfig> UEazyCommonActivatableWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(InputConfig, GameMouseCaptureMode);
}

#if WITH_EDITOR
void UEazyCommonActivatableWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	UEazyClassValidation::ValidateCompiledDefaultsForClass(this, CompileLog);
}
#endif // WITH_EDITOR

bool UEazyCommonActivatableWidget::TryHandleBackAction()
{
	return NativeOnHandleBackAction();
}

void UEazyCommonActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	NativeResetWidgetAnimationsState();
}

void UEazyCommonActivatableWidget::NativeConstruct()
{
	// We don't want to call CommonUI's NativeConstruct as it adds RegisterUIActionBinding stuff that we don't want with our input implementation.
	UUserWidget::NativeConstruct();
	
	InitializeViewModelsForWidget(this, ViewModels);
	
	if (bIsBackHandler)
	{
		if (UEazyGameUIPolicy* UIPolicy = UEazyGameUIPolicy::GetGameUIPolicy(this))
		{
			FEazyUIActionData BackActionData(UIPolicy->GetBackInputActionData());
			if (!OverrideBackActionDisplayName.IsEmpty())
			{
				BackActionData.DisplayName = OverrideBackActionDisplayName;
			}
			
			BackActionData.bDisplayInActionBar = bIsBackActionDisplayedInActionBar;
			
			RegisterInputActionBinding(FEazyInputActionBindingArgs(BackActionData, ETriggerEvent::Triggered, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
		}
	}

	if (bAutoActivate)
	{
		UE_LOGF(LogEazyUI, Verbose, "[%ls] auto-activated", *GetName());
		ActivateWidget();
	}
}

void UEazyCommonActivatableWidget::NativeResetWidgetAnimationsState()
{
	BP_ResetWidgetAnimationsState();
}

void UEazyCommonActivatableWidget::NativeDestruct()
{
	ReleaseViewModelsForWidget(this, ViewModels);
	RemoveAllInputActionBindingsForWidget(BoundInputActions);
	Super::NativeDestruct();
}

void UEazyCommonActivatableWidget::NativeOnPushed()
{
	BP_OnPushed();
	OnPushed().Broadcast();
}

FEazyActionBindingHandle UEazyCommonActivatableWidget::RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs)
{
	return RegisterInputActionBindingForWidget(*this, BindActionArgs, BoundInputActions);
}

void UEazyCommonActivatableWidget::RemoveInputActionBinding(const FEazyActionBindingHandle ActionBinding)
{
	RemoveInputActionBindingForWidget(ActionBinding, BoundInputActions);
}
