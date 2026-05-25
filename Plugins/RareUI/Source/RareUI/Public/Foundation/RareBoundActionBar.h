#pragma once

#include "Components/DynamicEntryBoxBase.h"
#include "RareBoundActionBar.generated.h"

class URareBoundActionButton;
enum class ECommonInputType : uint8;
struct FRareActionBindingHandle;
class ICommonBoundActionButtonInterface;
class IWidgetCompilerLog;

UCLASS(Blueprintable, ClassGroup=UI)
class RAREUI_API URareBoundActionBar : public UDynamicEntryBoxBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = CommonBoundActionBar)
	void SetDisplayOwningPlayerActionsOnly(bool bShouldOnlyDisplayOwningPlayerActions);

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif
	
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
	virtual void OnWidgetRebuilt() override;
	virtual void SynchronizeProperties() override;
	
	virtual void NativeOnActionButtonCreated(ICommonBoundActionButtonInterface* ActionButton, const FRareActionBindingHandle& RepresentedAction) {}

	virtual void ActionBarUpdateBeginImpl() {}
	virtual void ActionBarUpdateEndImpl() {}

	virtual UUserWidget* CreateActionButton(const FRareActionBindingHandle& BindingHandle);

	TSubclassOf<URareBoundActionButton> GetActionButtonClass() { return ActionButtonClass; }

private:
	void HandledInputTypeUpdated(ECommonInputType InputType);
	void HandleBoundActionsUpdated(bool bFromOwningPlayer);
	UFUNCTION()
	void HandleInputMappingsRebuiltUpdated();
	void UpdateDisplay();
	bool IsSafeToUpdateDisplay() const;
	void HandleDeferredDisplayUpdate();
	void HandlePlayerAdded(int32 PlayerIdx);
	void HandlePlayerRemoved(int32 PlayerIdx);

	void MonitorPlayerActions(const ULocalPlayer* NewPlayer);

	void ActionBarUpdateBegin();
	void ActionBarUpdateEnd();
	bool DoAnyActionButtonsHaveMouseCapture() const;

	UPROPERTY(EditAnywhere, Category = EntryLayout)
	TSubclassOf<URareBoundActionButton> ActionButtonClass;

	UPROPERTY(EditAnywhere, Category = Display)
	bool bDisplayOwningPlayerActionsOnly = true;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Display)
	bool bIgnoreDuplicateActions = true;

	bool bIsRefreshQueued = false;
};
