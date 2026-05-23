// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareLazyWidget.h"

#include "CommonLoadGuard.h"
#include "RarePrimaryGameLayout.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Foundation/RareCommonActivatableWidget.h"
#include "Foundation/RareCommonActivatableWidgetStack.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif // WITH_EDITOR

#if WITH_EDITOR
void URareLazyWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (LazyWidgetContent_Class.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has no LazyWidgetContent_Class selected in widget defaults.")), FText::FromString(GetName())));
	}
}
#endif // WITH_EDITOR

void URareLazyWidget::RequestConstructContent(const TFunction<void(UUserWidget*)>& OnConstructionCompleted)
{
	bShouldSetContent = true;
	LazyConstructWidget(OnConstructionCompleted);
}

void URareLazyWidget::RequestReleaseContent()
{
	bShouldSetContent = false;

	if (LoadGuard.IsValid())
	{
		LoadGuard->SetContent(SNullWidget::NullWidget);
	}

	if (!ContentWeak.IsValid())
	{
		// We've already released it, and we did the cleanup of slate content above this already.
		return;
	}

	if (OwningWidgetStackWeak.IsValid())
	{
		// Return widget back to the pool to be re-used later.
		OwningWidgetStackWeak->GetGeneratedWidgetsPool().Release(ContentWeak.Get(), true);
	}
	else
	{
		// We didn't use pooling, but we don't want to destroy the widget as we may navigate back to it. So don't destroy the widget here!
	}

	ContentWeak.Reset();
}

void URareLazyWidget::SetContentClass(const TSoftClassPtr<UUserWidget>& SoftWidget)
{
	LazyWidgetContent_Class = SoftWidget;
}

FDelegateHandle URareLazyWidget::CallAndRegister_ContentLoadedDelegate(const FLazyWidgetContentLoaded::FDelegate& Delegate)
{
	if (ContentWeak.IsValid())
	{
		Delegate.Execute(ContentWeak.Get());
	}
	
	return OnContentLoadedEvent.Add(Delegate);
}

void URareLazyWidget::UnregisterContentLoadedDelegate(const FDelegateHandle& InHandle)
{
	OnContentLoadedEvent.Remove(InHandle);
}

TSharedRef<SWidget> URareLazyWidget::RebuildWidget()
{
	if (IsDesignTime() && !LazyWidgetContent_Class.IsNull())
	{
		TSharedRef<SOverlay> MessageBox = SNew(SOverlay);

		MessageBox->AddSlot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text(FText::FromString(LazyWidgetContent_Class.GetAssetName()))
			];

		return MessageBox;
	}

	LoadGuard = SNew(SLoadGuard);
	LoadGuard->SetForceShowSpinner(true);
	
	if (ContentWeak.IsValid())
	{
		LoadGuard->SetContent(ContentWeak->TakeWidget());
		LoadGuard->SetForceShowSpinner(false);
	}

	return LoadGuard.ToSharedRef();
}

void URareLazyWidget::ReleaseSlateResources(const bool bReleaseChildren)
{
	LoadGuard.Reset();
	
	Super::ReleaseSlateResources(bReleaseChildren);
}

void URareLazyWidget::LazyConstructWidget(const TFunction<void(UUserWidget*)>& OnWidgetContentLoaded)
{
	if (IsDesignTime())
	{
		return;
	}

	if (bShouldSyncLoad)
	{
		UAssetManager::GetStreamableManager().RequestSyncLoad(LazyWidgetContent_Class.ToSoftObjectPath());
		ensureAlwaysMsgf(LazyWidgetContent_Class.Get(), TEXT("Failed to sync load %s"), *LazyWidgetContent_Class.ToSoftObjectPath().ToString());

		const TSubclassOf<UUserWidget> LoadedWidget = LazyWidgetContent_Class.Get();
		UUserWidget* ConstructedWidget = SetLoadedContent(LoadedWidget);

		if (OnWidgetContentLoaded != nullptr)
		{
			OnWidgetContentLoaded(ConstructedWidget);
		}
		
		return;
	}

	FName SuspendInputToken;
	if (bShouldSuspendInputDuringLoad)
	{
		SuspendInputToken = UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), "ConstructingLazyWidget");
	}
	
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	StreamingHandle = StreamableManager.RequestAsyncLoad(LazyWidgetContent_Class.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
	[OnWidgetContentLoaded, WeakThis = MakeWeakObjectPtr(this), SuspendInputToken]
	{
		 if (!WeakThis.IsValid())
		 {
		 	return;
		 }
		
		 const TSubclassOf<UUserWidget> LoadedWidget = WeakThis->LazyWidgetContent_Class.Get();
		 if (ensureMsgf(LoadedWidget, TEXT("Failed to async load %s"), *WeakThis->LazyWidgetContent_Class.ToSoftObjectPath().ToString()))
		 {
			 UUserWidget* ConstructedWidget = WeakThis->SetLoadedContent(LoadedWidget);

			 if (OnWidgetContentLoaded != nullptr)
			 {
			 	OnWidgetContentLoaded(ConstructedWidget);
			 }
		 }

		 if (WeakThis->bShouldSuspendInputDuringLoad)
		 {
		 	UCommonUIExtensions::ResumeInputForPlayer(WeakThis->GetOwningPlayer(), SuspendInputToken);
		 }
	}), FStreamableManager::AsyncLoadHighPriority);

	// Set up a cancel delegate so that we can resume input if this handler is canceled.
	StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
	  [this, SuspendInputToken]
	  {
		  UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);
			UE_LOG(LogRareUI, Display, TEXT("[RareLazyWidget] Async load of a %s widget was cancelled"), *LazyWidgetContent_Class.GetAssetName());
	  }));
}

UUserWidget* URareLazyWidget::SetLoadedContent(const TSubclassOf<UUserWidget>& InContent)
{
	// We first try to create the widget on the container stack, if it exists. So that we can pool the widget instead of recreating it all the time.
	const bool bSuccess = TryCreateWidgetOnOwningContainerStack(InContent);

	if (!bSuccess)
	{
		// Apparently we're not owned by an ActivatableWidgetStack, so let's just hard create the widget without any pooling and store it in a hard reference.
		if (!IsValid(ContentHardRef))
		{
			// We haven't created the widget yet, so let's create it now.
			ContentHardRef = CreateWidget<UUserWidget>(GetTypedOuter<UUserWidget>(), InContent);
		}

		ContentWeak = ContentHardRef;
	}

	OnContentLoadedEvent.Broadcast(ContentWeak.Get());

	if (LoadGuard.IsValid() && bShouldSetContent)
	{
		LoadGuard->SetContent(ContentWeak.IsValid() ? ContentWeak->TakeWidget() : SNullWidget::NullWidget);
		LoadGuard->SetForceShowSpinner(false);
	}

	return ContentWeak.Get();
}

bool URareLazyWidget::TryCreateWidgetOnOwningContainerStack(const TSubclassOf<UUserWidget>& InContent)
{
	if (URareCommonActivatableWidgetStack* OwningWidgetStack = GetOwningWidgetStack())
	{
		ContentWeak = OwningWidgetStack->GetGeneratedWidgetsPool().GetOrCreateInstance(InContent);
		OwningWidgetStackWeak = OwningWidgetStack;

		return ContentWeak.IsValid();
	}

	return false;
}

URareCommonActivatableWidgetStack* URareLazyWidget::GetOwningWidgetStack() const
{
	const UCommonActivatableWidget* OwningActivatableWidget = GetTypedOuter<UCommonActivatableWidget>();
	if (!IsValid(OwningActivatableWidget))
	{
		return nullptr;
	}

	const URarePrimaryGameLayout* PrimaryGameLayout = URarePrimaryGameLayout::GetPrimaryGameLayout(OwningActivatableWidget->GetOwningPlayer());
	if (!IsValid(PrimaryGameLayout))
	{
		return nullptr;
	}

	const UCommonActivatableWidget* CurrentWidget = OwningActivatableWidget;

	while (CurrentWidget)
	{
		UCommonActivatableWidgetContainerBase* WidgetContainer = PrimaryGameLayout->GetOwningContainerStack(CurrentWidget);
		if (IsValid(WidgetContainer))
		{
			return Cast<URareCommonActivatableWidgetStack>(WidgetContainer);
		}

		CurrentWidget = CurrentWidget->GetTypedOuter<UCommonActivatableWidget>();
	}

	return nullptr;
}
