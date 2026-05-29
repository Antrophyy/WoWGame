#include "Foundation/EazyCommonActivatableWidget.h"

#include "LogEazyUI.h"
#include "Routing/EazyUIActionRouterBase.h"
#include "EazyClassValidation.h"
#include "EazyGameUIPolicy.h"

bool UEazyCommonActivatableWidget::TryHandleBackAction()
{
	return NativeOnHandleBackAction();
}

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

void UEazyCommonActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	NativeResetWidgetAnimationsState();
}

void UEazyCommonActivatableWidget::NativeConstruct()
{
	// We don't want to call CommonUI's NativeConstruct as it adds RegisterUIActionBinding stuff that we don't want with our input implementation.
	UUserWidget::NativeConstruct();
	
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
	RemoveAllInputActionBindings();
	Super::NativeDestruct();
}

FEazyActionBindingHandle UEazyCommonActivatableWidget::RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs)
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

	return FEazyActionBindingHandle{};
}

void UEazyCommonActivatableWidget::RemoveInputActionBinding(FEazyActionBindingHandle ActionBinding)
{
	const int32 BindingIndex = BoundInputActions.IndexOfByKey(ActionBinding);
	if (BindingIndex == INDEX_NONE)
	{
		return;
	}

	BoundInputActions[BindingIndex].Unregister();

	BoundInputActions.RemoveAtSwap(BindingIndex);
}

void UEazyCommonActivatableWidget::RemoveAllInputActionBindings()
{
	for (FEazyActionBindingHandle& Handle : BoundInputActions)
	{
		Handle.Unregister();
	}

	BoundInputActions.Empty();
}

AHUD* UEazyCommonActivatableWidget::GetHUD() const
{
	const APlayerController* OwningPlayerController = GetOwningPlayer();
	return IsValid(OwningPlayerController) ? OwningPlayerController->GetHUD() : nullptr;
}
