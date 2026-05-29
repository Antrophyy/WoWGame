#pragma once

#include "Input/CommonUIActionRouterBase.h"
#include "Routing/EazyActionBinding.h"
#include "Routing/EazyInputActionBindingArgs.h"
#include "UObject/ObjectKey.h"
#include "UObject/WeakInterfacePtr.h"
#include "EazyUIActionRouterBase.generated.h"

class IEazyGamepadValuesHandler;
class FEazyInputPreprocessor;
class FEazyAnalogCursor;
class UInputAction;
class UCommonActivatableWidget;
class UEnhancedInputComponent;
class APlayerController;

UCLASS(ClassGroup=UI)
class EAZYUIINPUT_API UEazyUIActionRouterBase : public UCommonUIActionRouterBase
{
	GENERATED_BODY()

public:
	// Convenience getter that casts the CommonUI router to UEazyUIActionRouterBase. Returns nullptr if not found or wrong type.
	static UEazyUIActionRouterBase* Get(const UWidget& ContextWidget);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SetAnalogMovementEnabled(const bool bEnable) const;

	void AddGamepadValuesListener(const TWeakInterfacePtr<IEazyGamepadValuesHandler> InHandler) const;
	void RemoveGamepadValuesListener(const TWeakInterfacePtr<IEazyGamepadValuesHandler> InHandler) const;

	FEazyActionBindingHandle RegisterInputAction(const UWidget& Widget, const FEazyInputActionBindingArgs& BindActionArgs);
	const TArray<FEazyActionBindingHandle>& GetActiveBindings() const;

	void UnregisterInputAction(FEazyActionBindingHandle& Handle);

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

	// Called when the leafmost active node in the active root tree changes. Refreshes the active bindings cache.
	void HandleLeafmostActiveNodeChangedInternal();

	UEnhancedInputComponent* GetEnhancedInputComponent() const;
	UCommonActivatableWidget* ResolveOwningActivatable(const UWidget& Widget) const;
	void RefreshActiveBindingsCache();
	bool IsBindingActive(const FEazyActionBinding& Binding) const;

	// ── Binding storage ──────────────────────────────────────────────────────────
	// All registered bindings represented as handles. Binding data is owned by FEazyActionBinding static storage.
	TArray<FEazyActionBindingHandle> AllActionBindings;

	// Cached active binding handles — rebuilt whenever the leafmost active node changes.
	TArray<FEazyActionBindingHandle> ActiveBindings;

	// Active bindings indexed by (InputAction, TriggerEvent) for fast dispatch.
	TMap<TPair<TObjectKey<const UInputAction>, ETriggerEvent>, TArray<FEazyActionBindingHandle>> ActiveBindingsByInputAction;

	// Tracks the Enhanced Input handle per unique (action, trigger) so we bind only once per pair.
	TMap<TPair<TObjectKey<const UInputAction>, ETriggerEvent>, uint32> UniqueEnhancedInputHandles;

	int32 NextBindingId = 0;

	// ── Existing infrastructure ──────────────────────────────────────────────
	TSharedPtr<FEazyAnalogCursor> AnalogCursor;
	TSharedPtr<FEazyInputPreprocessor> EazyInputPreprocessor;
};
