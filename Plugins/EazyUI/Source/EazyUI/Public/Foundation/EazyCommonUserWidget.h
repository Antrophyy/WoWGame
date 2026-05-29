#pragma once

#include "CommonUserWidget.h"
#include "Routing/EazyActionBindingHandle.h"
#include "Routing/EazyInputActionBindingArgs.h"
#include "EazyCommonUserWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyCommonUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// Validate properties that should be set in the editor. Loops through all properties with tag "validate" or "validateIgnoreParent" and checks if they are set.
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

	// Registers an Enhanced Input action binding on this widget and stores the handle.
	FEazyActionBindingHandle RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs);

	// Removes a previously registered input action binding.
	void RemoveInputActionBinding(FEazyActionBindingHandle ActionBinding);

	// Removes all bindings registered by this widget.
	void RemoveAllInputActionBindings();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected: // Overridable Functions
	
	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, Category= AnimationStates, meta = (DisplayName = "Reset Widget Animations State"))
	void BP_ResetWidgetAnimationsState();

	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	virtual void NativeResetWidgetAnimationsState();

protected:
	// Gets the HUD associated with Player Controller.
	AHUD* GetHUD() const;

	// Gets the HUD associated with Player Controller. cast to the template type.
	template <class T>
	T* GetHUD() const
	{
		return Cast<T>(GetHUD());
	}

private:
	// Handles for actions registered through RegisterInputActionBinding.
	TArray<FEazyActionBindingHandle> BoundInputActions;

};
