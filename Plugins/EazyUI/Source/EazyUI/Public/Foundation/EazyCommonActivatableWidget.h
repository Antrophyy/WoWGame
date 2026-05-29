#pragma once

#include "CommonActivatableWidget.h"
#include "Routing/EazyActionBindingHandle.h"
#include "Routing/EazyInputActionBindingArgs.h"
#include "EazyCommonActivatableWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyCommonActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// Function used when trying to call back action internally on another widget. (e.g. giving another widget a chance to handle a back action before the owning widget does)
	// @return True if Handled, False if Unhandled
	bool TryHandleBackAction();
	
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	
	FSimpleMulticastDelegate& OnPushed() const { return OnPushedEvent; }

#if WITH_EDITOR
	// Validate properties that should be set in the editor. Loops through all properties with tag "validate" or "validateIgnoreParent" and checks if they are set.
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR
	
protected:
	virtual void NativeOnActivated() override;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected: // Overrideable Functions

	// Called on activated. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, Category= AnimationStates, meta = (DisplayName = "Reset Widget Animations State"))
	void BP_ResetWidgetAnimationsState();

	// Called on activated. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	virtual void NativeResetWidgetAnimationsState();
	
	// Called when this widget is pushed to a stack. Unlike NativeOnActivated, this is only called when PushWidgetToLayer is called.
	// Activation is called even when you navigate back to this widget via back action.
	virtual void NativeOnPushed();
	
	// Called when this widget is pushed to a stack. Unlike NativeOnActivated, this is only called when PushWidgetToLayer is called.
	// Activation is called even when you navigate back to this widget via back action.
	UFUNCTION(BlueprintImplementableEvent, Category = ActivatableWidget, meta = (DisplayName = "On Pushed"))
	void BP_OnPushed();
	
protected:
	
	// Registers an Enhanced Input action binding on this widget and stores the handle.
	FEazyActionBindingHandle RegisterInputActionBinding(const FEazyInputActionBindingArgs& BindActionArgs);
	
	void RemoveInputActionBinding(FEazyActionBindingHandle ActionBinding);

	// Removes all bindings registered by this widget.
	void RemoveAllInputActionBindings();
	
	// Gets the HUD associated with Player Controller.
	AHUD* GetHUD() const;

	// Gets the HUD associated with Player Controller. cast to the template type.
	template <class T>
	T* GetHUD() const
	{
		return Cast<T>(GetHUD());
	}

protected: // Set by the Editor
	
	// The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller?
	UPROPERTY(EditAnywhere, Category=Input)
	ECommonInputMode InputConfig = ECommonInputMode::Menu;

	// The desired mouse behavior when the game gets input.
	UPROPERTY(EditAnywhere, Category=Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::NoCapture;

private:
	// Handles for actions registered through RegisterInputActionBinding.
	TArray<FEazyActionBindingHandle> BoundInputActions;
	
	mutable FSimpleMulticastDelegate OnPushedEvent;
	
	friend class UEazyPrimaryGameLayout;
};
