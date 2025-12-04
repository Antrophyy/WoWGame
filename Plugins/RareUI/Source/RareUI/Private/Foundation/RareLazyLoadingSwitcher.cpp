#include "Foundation/RareLazyLoadingSwitcher.h"

#include "CommonUIUtils.h"
#include "Foundation/RareCommonActivatableWidget.h"
#include "Foundation/RareLazyWidget.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif // WITH_EDITOR

#if WITH_EDITOR
void URareLazyLoadingSwitcher::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	// Check if the children of the switcher are all RareLazyWidgets
	TArray<UWidget*> Children = GetAllChildren();
	for (UWidget* Child : Children)
	{
		const URareLazyWidget* LazyWidget = Cast<URareLazyWidget>(Child);
		if (!IsValid(LazyWidget))
		{
			CompileLog.Error(FText::Format(
				FText::FromString(TEXT("{0} has a child widget that is not a RareLazyWidget, please replace it.")),
				FText::FromString(GetName())));
		}
	}
}
#endif //WITH_EDITOR

URareLazyLoadingSwitcher::URareLazyLoadingSwitcher(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Animated switcher doesn't play nice with focus on activatable screens. So we disable it.
	TransitionDuration = 0;
}

void URareLazyLoadingSwitcher::SetActiveWidget(UWidget* Widget)
{
	NavigateToWidget(Widget);

	Super::SetActiveWidget(Widget);
}

void URareLazyLoadingSwitcher::SetActiveWidgetIndex(const int32 Index)
{
	NavigateToWidget(GetChildAt(Index));

	Super::SetActiveWidgetIndex(Index);
}

void URareLazyLoadingSwitcher::SetIsBeingDestroyed(const bool bInIsBeingDestroyed)
{
	bIsBeingDestroyed = bInIsBeingDestroyed;
}

void URareLazyLoadingSwitcher::ReleaseSlateResources(const bool bReleaseChildren)
{
	URareLazyWidget* OldActiveWidget = Cast<URareLazyWidget>(GetActiveWidget());
	DeactivateWidget(OldActiveWidget);

	Super::ReleaseSlateResources(bReleaseChildren);
}

void URareLazyLoadingSwitcher::NavigateToWidget(UWidget* InWidget)
{
	if (!IsValid(InWidget))
	{
		return;
	}

	if (DesiredLazyWidget.IsValid() && DesiredLazyWidget == InWidget && IsValid(DesiredLazyWidget->GetContent()))
	{
		// Already active and constructed, no need to re-activate.
		return;
	}
	
	URareLazyWidget* OldActiveWidget = Cast<URareLazyWidget>(GetActiveWidget());
	DeactivateWidget(OldActiveWidget);

	if (bIsBeingDestroyed)
	{
		return;
	}

	URareLazyWidget* NewLazyWidget = Cast<URareLazyWidget>(InWidget);
	DesiredLazyWidget = NewLazyWidget;
	ActivateWidget(NewLazyWidget);
}

void URareLazyLoadingSwitcher::ActivateWidget(const URareLazyWidget* LazyWidget) const
{
	if (!IsValid(LazyWidget))
	{
		ensureAlwaysMsgf(false, TEXT("Casting child widget of Rare Lazy Loading Switcher failed, child is not of type RareLazyWidget. This switcher is for lazy loading only."));
		return;
	}

	DesiredLazyWidget->RequestConstructContent([WeakThis = MakeWeakObjectPtr(this)](UUserWidget* ConstructedWidget)
	{
		if (!WeakThis.IsValid())
			return;

		if (WeakThis->DesiredLazyWidget->GetContent() != ConstructedWidget)
		{
			// User loaded some other tab before we had a chance to activate, don't activate us then.
			return;
		}

		if (UCommonActivatableWidget* ActivatableWidget = Cast<UCommonActivatableWidget>(ConstructedWidget))
		{
			ActivatableWidget->ActivateWidget();
		}
	});
}

void URareLazyLoadingSwitcher::DeactivateWidget(URareLazyWidget* LazyWidget) const
{
	if (!IsValid(LazyWidget))
		return;

	if (UCommonActivatableWidget* ActivatableWidget = Cast<UCommonActivatableWidget>(LazyWidget->GetContent()))
	{
		ActivatableWidget->DeactivateWidget();
	}

	LazyWidget->RequestReleaseContent();
}
