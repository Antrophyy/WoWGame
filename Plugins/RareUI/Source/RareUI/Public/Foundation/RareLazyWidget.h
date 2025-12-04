#pragma once

#include "Components/Widget.h"

#include "RareLazyWidget.generated.h"

struct FStreamableHandle;
class URareCommonActivatableWidgetStack;
class SLoadGuard;

DECLARE_MULTICAST_DELEGATE_OneParam(FLazyWidgetContentLoaded,  UUserWidget* ContentWidget);

UCLASS(NotBlueprintable, ClassGroup=UI)
class RAREUI_API URareLazyWidget : public UWidget
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

	// Attempts to load the content (if not yet loaded) and creates a widget (if not yet created). Uses pooling if possible.
	void RequestConstructContent(const TFunction<void(UUserWidget*)>& OnWidgetContentLoaded = nullptr);

	// Attempts to return the content to the pool (if it was created on a pool) and releases the slate resources.
	void RequestReleaseContent();

	// Possible to change lazy widget class at runtime.
	void SetContentClass(const TSoftClassPtr<UUserWidget>& SoftWidget);

	// Attempts to load the content (if not yet loaded) and creates a widget (if not yet created). Uses pooling if possible.
	FDelegateHandle CallAndRegister_ContentLoadedDelegate(FLazyWidgetContentLoaded::FDelegate Delegate);
	void UnregisterContentLoadedDelegate(FDelegateHandle& InHandle);

	// Returns the loaded content, if any.
	UUserWidget* GetContent() const { return ContentWeak.Get(); }

	// There is a possibility of async load flush and as such should not be used. You should rather use async loading.
	UPROPERTY(EditAnywhere, Category = "Load Properties")
	bool bShouldSyncLoad = true;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	// Constructs a widget on demand.
	void LazyConstructWidget(const TFunction<void(UUserWidget*)>& OnWidgetContentLoaded);

protected: // Set By The Editor
	
	UPROPERTY(EditAnywhere, Category = "Load Properties")
	TSoftClassPtr<UUserWidget> LazyWidgetContent_Class;

private: // Internal

	UUserWidget* SetLoadedContent(const TSubclassOf<UUserWidget>& InContent);

	// Attempt to look through the hierarchy if we can find a stack and use its pool to get or create instance.
	bool TryCreateWidgetOnOwningContainerStack(const TSubclassOf<UUserWidget>& InContent);
	URareCommonActivatableWidgetStack* GetOwningWidgetStack() const;

	TSharedPtr<SLoadGuard> LoadGuard;
	TWeakObjectPtr<UUserWidget> ContentWeak;
	FLazyWidgetContentLoaded OnContentLoadedEvent;
	
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ContentHardRef;
	
	TWeakObjectPtr<URareCommonActivatableWidgetStack> OwningWidgetStackWeak;

	TSharedPtr<FStreamableHandle> StreamingHandle;

	// For cases when someone went to a different tab while we haven't yet loaded fully.
	bool bShouldSetContent = false;
};
