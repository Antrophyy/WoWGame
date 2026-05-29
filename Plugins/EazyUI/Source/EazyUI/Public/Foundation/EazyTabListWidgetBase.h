#pragma once

#include "CommonTabListWidgetBase.h"
#include "EazyUIActionData.h"
#include "Routing/EazyActionBindingHandle.h"
#include "EazyTabListWidgetBase.generated.h"

class UEazyEnhancedActionWidget;
class UStackBox;
class UEazyLazyWidget;
class UCommonButtonBase;
class UHorizontalBox;
class UEazyLazyLoadingSwitcher;

USTRUCT(BlueprintType, Category=UI)
struct FEazyTabDescriptor
{
	GENERATED_BODY()

public:
	// ID for this tab.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Setup)
	FName TabId;

	// Text that names this tab.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setup)
	FText TabText;

	// Icon to display for this tab.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setup)
	TObjectPtr<UTexture2D> TabIconTexture;
	
	// If true, this tab is registered when RegisterDynamicTab is called. Otherwise, it will not be added or initialized.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Setup)
	bool bRegistered = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Setup)
	bool bShouldSyncLoad = true;

	// Type of button to make for this tab.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Setup)
	TSubclassOf<UCommonButtonBase> TabButtonType;

	// Content widget that belongs to this tab.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Setup)
	TSoftClassPtr<UUserWidget> TabContentType;
};

UCLASS(Abstract, ClassGroup= UI)
class EAZYUI_API UEazyTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public: // Exposed

	UWidget* GetActiveTabContent() const;

	// Toggles whether a specified tab is registered; can only be called before the switcher is associated.
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SetTabRegisteredState(FName TabNameId, bool bRegistered);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SelectTabByIndex(const int32 Index, bool bSuppressClickFeedback = false);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsFirstTabActive() const;

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsLastTabActive() const;

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsTabVisible(FName TabId) const;

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	int32 GetVisibleTabCount() const;

	void RegisterDynamicTab(const FEazyTabDescriptor& TabInfo);

	void ToggleTabListEnable(bool bEnabled);

	virtual void SynchronizeProperties() override;
	
	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;
	virtual void HandleTabRemoval_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	FEazyTabDescriptor GetTabInfoById(FName TabId) const;

	virtual void UpdateTabVisuals(UUserWidget* TabButton) const;
	virtual void UpdateBindings() override;
	
	// TODO: It is unclear why these functions are not protected in the parent, but once they are, they can be replaced.
	virtual void HandleNextTabActionPressed();
	virtual void HandlePrevTabActionPressed();
	
protected: // Set by the Editor

	// Anim Switcher in the hierarchy that's going to be used to display content.
	UPROPERTY(EditAnywhere, Category="Setup|General")
	TObjectPtr<UCommonAnimatedSwitcher> AnimSwitcher;

	// If you already know which tabs and content you want to use, you can set them up here instead of doing it dynamically (most of the time you know it beforehand).
	UPROPERTY(EditAnywhere, Category="Setup|General")
	TArray<FEazyTabDescriptor> PreregisteredTabInfoArray;

	// Defines whether the tabs should take all available space and be spaced equally or if they are aligned center
	UPROPERTY(EditAnywhere, Category="Setup|Layout")
	bool bTakeAvailableSpace = false;

	// The spacing between every tab.
	UPROPERTY(EditAnywhere, Category="Setup|Layout")
	float TabsSpacing = 0.f;

	// The spacing around the container that holds the tabs.
	UPROPERTY(EditAnywhere, Category="Setup|Layout")
	float ActionsSpacing = 0.f;

	// Should the TabList be horizontal or vertical?
	UPROPERTY(EditAnywhere, Category="Setup|Layout")
	TEnumAsByte<EOrientation> Orientation = Orient_Horizontal;
	
	UPROPERTY(EditAnywhere, Category="Setup|Input")
	FEazyUIActionData NextTab_InputAction;
	
	UPROPERTY(EditAnywhere, Category="Setup|Input")
	FEazyUIActionData PrevTab_InputAction;

#if WITH_EDITORONLY_DATA

	// How many tabs should be created in the designer.
	UPROPERTY(EditAnywhere, Category="Setup|Debug")
	int32 DebugTabCount = 4;

	// Which Widget Blueprint is used to display the tab visual in the designer.
	UPROPERTY(EditAnywhere, Category="Setup|Debug")
	TSubclassOf<UCommonButtonBase> DebugTab_Class;

#endif // WITH_EDITORONLY_DATA

private: // Internal

	void SetupPreregisteredTabs();
	void SetupDynamicTabs();
	void SetupTab(const FEazyTabDescriptor& TabInfo);
	void UpdatePaddings() const;

#if WITH_EDITOR
	void Debug_CreateTabsDesigner();
#endif // WITH_EDITOR

	UPROPERTY()
	TMap<FName, FEazyTabDescriptor> DynamicTabInfoMap;
	
	FEazyActionBindingHandle NextTabActionHandle;
	FEazyActionBindingHandle PrevTabActionHandle;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStackBox> TabsContainer_StackBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStackBox> Root_StackBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEazyEnhancedActionWidget> PrevTabAction;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEazyEnhancedActionWidget> NextTabAction;
};
