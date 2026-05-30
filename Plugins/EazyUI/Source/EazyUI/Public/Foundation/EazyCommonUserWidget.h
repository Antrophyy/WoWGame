#pragma once

#include "CommonUserWidget.h"
#include "Input/EazyInputBindingWidgetInterface.h"
#include "EazyViewModelWidgetInterface.h"
#include "Routing/EazyActionBindingHandle.h"
#include "Routing/EazyInputActionBindingArgs.h"
#include "EazyCommonUserWidget.generated.h"

class UEazyViewModelBase;

UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyCommonUserWidget : public UCommonUserWidget,
                                         public IEazyViewModelWidgetInterface,
                                         public IEazyInputBindingWidgetInterface
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// Validate properties that should be set in the editor. Loops through all properties with tag "validate" or "validateIgnoreParent" and checks if they are set.
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected: // Overridable Functions

	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, Category= AnimationStates, meta = (DisplayName = "Reset Widget Animations State"))
	void BP_ResetWidgetAnimationsState();

	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	virtual void NativeResetWidgetAnimationsState();

protected: // Input

	// Registers an Enhanced Input action binding on this widget and stores the handle.
	FEazyActionBindingHandle RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs);

	// Removes a previously registered input action binding.
	void RemoveInputActionBinding(FEazyActionBindingHandle ActionBinding);

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UEazyViewModelBase>> ViewModels;

	// Handles for actions registered through RegisterInputActionBinding.
	TArray<FEazyActionBindingHandle> BoundInputActions;
};
