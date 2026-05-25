
#pragma once

#include "Input/CommonUIActionRouterBase.h"
#include "Routing/RareActionBinding.h"
#include "Routing/RareInputActionBindingArgs.h"
#include "InputAction.h"
#include "UObject/WeakInterfacePtr.h"
#include "RareUIActionRouterBase.generated.h"

class IRareGamepadValuesHandler;
class FRareInputPreprocessor;
class FRareAnalogCursor;
class UInputAction;
class UCommonActivatableWidget;
class UEnhancedInputComponent;
class APlayerController;

UCLASS(ClassGroup=UI)
class RAREINPUT_API URareUIActionRouterBase : public UCommonUIActionRouterBase
{
	GENERATED_BODY()

public:
	/** Convenience getter that casts the CommonUI router to URareUIActionRouterBase. Returns nullptr if not found or wrong type. */
	static URareUIActionRouterBase* Get(const UWidget& ContextWidget);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SetAnalogMovementEnabled(const bool bEnable) const;

	void AddGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const;
	void RemoveGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const;

	FRareActionBindingHandle RegisterInputAction(const UWidget& Widget, const FRareInputActionBindingArgs& BindActionArgs);
	const TArray<FRareActionBindingHandle>& GetActiveBindings() const;

	void UnregisterInputAction(FRareActionBindingHandle& Handle);

	virtual FUIActionBindingHandle RegisterUIActionBinding(const UWidget& Widget, const FBindUIActionArgs& BindActionArgs) override;
	
	

protected:
	virtual TSharedRef<FCommonAnalogCursor> MakeAnalogCursor() const override;
	virtual void SetActiveRoot(FActivatableTreeRootPtr NewActiveRoot) override;

private:
	// Called by the lambda we bind to UEnhancedInputComponent; dispatches to all matching bindings.
	void ProcessEnhancedInputAction(const FInputActionInstance& ActionInstance);

	// Binds *all* currently registered unique (action, event) pairs to the given EIC.
	void BindAllToEnhancedInputComponent(UEnhancedInputComponent* EnhancedInputComponent);

	// Removes all EI bindings (e.g. before the component goes away or on shutdown).
	void UnbindAllFromEnhancedInputComponent();

	// Ensures (Action, Event) has exactly one EI binding on EIC.
	void EnsureUniqueEIBinding(const UInputAction* Action, ETriggerEvent Event, UEnhancedInputComponent* EnhancedInputComponent);

	// Removes the EI binding for (Action, Event) if no more internal bindings reference it.
	void RemoveUniqueEIBinding(const UInputAction* Action, ETriggerEvent Event);

	void HandlePlayerControllerChanged(APlayerController* NewController);

	/** Called when the leafmost active node in the active root tree changes. Refreshes the active bindings cache. */
	void HandleLeafmostActiveNodeChangedInternal();

	UEnhancedInputComponent* GetEnhancedInputComponent() const;
	UCommonActivatableWidget* ResolveOwningActivatable(const UWidget& Widget) const;
	void RefreshActiveBindingsCache();
	bool IsBindingActive(const FRareActionBinding& Binding) const;

	// ── Binding storage ──────────────────────────────────────────────────────────
	/** All registered bindings represented as handles. Binding data is owned by FRareActionBinding static storage. */
	TArray<FRareActionBindingHandle> AllActionBindings;

	// Cached active binding handles — rebuilt whenever the leafmost active node changes.
	TArray<FRareActionBindingHandle> ActiveBindings;

	// Tracks the Enhanced Input handle per unique (action, trigger) so we bind only once per pair.
	TMap<TPair<TObjectPtr<const UInputAction>, ETriggerEvent>, uint32> UniqueEnhancedInputHandles;

	int32 NextBindingId = 0;

	// ── Existing infrastructure ──────────────────────────────────────────────
	TSharedPtr<FRareAnalogCursor> AnalogCursor;
	TSharedPtr<FRareInputPreprocessor> RareInputPreprocessor;
};


