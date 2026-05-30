#include "Core/EazyViewModelWidgetInterface.h"

#include "Blueprint/UserWidget.h"
#include "Core/EazyViewModelBase.h"
#include "Core/EazyViewModelSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "MVVMBlueprintLibrary.h"
#include "View/MVVMView.h"
#include "View/MVVMViewClass.h"

void IEazyViewModelWidgetInterface::InitializeViewModelsForWidget(UUserWidget* Widget, TArray<TObjectPtr<UEazyViewModelBase>>& InOutViewModels) const
{
	if (!IsValid(Widget))
	{
		return;
	}

	const UMVVMView* View = Widget->GetExtension<UMVVMView>();
	if (!IsValid(View))
	{
		return;
	}

	const UMVVMViewClass* ViewClass = View->GetViewClass();
	if (!IsValid(ViewClass))
	{
		return;
	}

	UEazyViewModelSubsystem* ViewModelSubsystem = ULocalPlayer::GetSubsystem<UEazyViewModelSubsystem>(Widget->GetOwningLocalPlayer());
	if (!IsValid(ViewModelSubsystem))
	{
		return;
	}

	for (const FMVVMViewClass_Source& Source : ViewClass->GetSources())
	{
		UEazyViewModelBase* ViewModel = ViewModelSubsystem->GetOrCreateViewModel(Widget, Source.GetSourceClass());

		// Keep references on the widget for the lifetime of this widget instance.
		InOutViewModels.Emplace(ViewModel);
		UMVVMBlueprintLibrary::SetViewModelByClass(Widget, ViewModel);
	}
}

void IEazyViewModelWidgetInterface::ReleaseViewModelsForWidget(UUserWidget* Widget, TArray<TObjectPtr<UEazyViewModelBase>>& InOutViewModels) const
{
	if (!IsValid(Widget))
	{
		return;
	}

	UEazyViewModelSubsystem* ViewModelSubsystem = ULocalPlayer::GetSubsystem<UEazyViewModelSubsystem>(Widget->GetOwningLocalPlayer());
	if (!IsValid(ViewModelSubsystem))
	{
		return;
	}

	for (const TObjectPtr<UEazyViewModelBase>& ViewModel : InOutViewModels)
	{
		if (!IsValid(ViewModel))
		{
			continue;
		}

		if (ViewModel->AllowMultipleInstances() && !ViewModel->IsPersistent())
		{
			ViewModelSubsystem->DestroyViewModel(ViewModel);
			continue;
		}

		const TSubclassOf<UEazyViewModelBase> ViewModelClass = ViewModel->GetClass();
		ViewModelSubsystem->ReleaseViewModel(Widget, ViewModelClass);
	}

	InOutViewModels.Reset();
}
