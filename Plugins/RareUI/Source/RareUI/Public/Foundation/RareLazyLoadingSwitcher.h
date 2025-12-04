#pragma once

#include "CommonAnimatedSwitcher.h"
#include "Blueprint/UserWidgetPool.h"
#include "RareLazyLoadingSwitcher.generated.h"

class URareLazyWidget;

/*
 * This switcher uses the RareLazyWidget class to Load the desired page content once the active index of the switcher has been changed
 */
UCLASS(NotBlueprintable, ClassGroup=UI)
class RAREUI_API URareLazyLoadingSwitcher : public UCommonAnimatedSwitcher
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR
	
	URareLazyLoadingSwitcher(const FObjectInitializer& ObjectInitializer);

	virtual void SetActiveWidget(UWidget* Widget) override;
	virtual void SetActiveWidgetIndex(const int32 Index) override;

	void SetIsBeingDestroyed(const bool bInIsBeingDestroyed);
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	void NavigateToWidget(UWidget* InWidget);

	void ActivateWidget(const URareLazyWidget* LazyWidget) const;
	void DeactivateWidget(URareLazyWidget* LazyWidget) const;

	bool bIsBeingDestroyed = false;

	TWeakObjectPtr<URareLazyWidget> DesiredLazyWidget;
};
