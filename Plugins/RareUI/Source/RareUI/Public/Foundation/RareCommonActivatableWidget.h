#pragma once

#include "CommonActivatableWidget.h"
#include "RareCommonActivatableWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class RAREUI_API URareCommonActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	/**
	 * Function used when trying to call back action internally on another widget. (e.g. Giving the chance to another widget to handle a back action before the owning widget does)
	 * @return True if Handled, False if Unhandled
	 */
	bool TryHandleBackAction();
	
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

#if WITH_EDITOR
	// Validate properties that should be set in the editor. Loops through all properties with tag "validate" or "validateIgnoreParent" and checks if they are set.
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR
	
protected:
	virtual void NativeOnActivated() override;

protected: // Overrideable Functions

	// Called on activated. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, Category= AnimationStates, meta = (DisplayName = "Reset Widget Animations State"))
	void BP_ResetWidgetAnimationsState();

	// Called on activated. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
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

protected: // Set by the Editor

	// When a widget gets activated, it tries to change input mode and mouse capture mode.
	// For some widgets, we don't want this behaviour (e.g. when the widget is a child of another widget that already does this).
	// Useful for widgets that just want to handle registered actions without having to register/unregister them all the time.
	UPROPERTY(EditAnywhere, Category=Input)
	bool bShouldAffectInputMode = true;
	
	// The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller?
	UPROPERTY(EditAnywhere, Category=Input, meta = (EditCondition = "bShouldAffectInputMode"))
	ECommonInputMode InputConfig = ECommonInputMode::Menu;

	// The desired mouse behavior when the game gets input.
	UPROPERTY(EditAnywhere, Category=Input, meta = (EditCondition = "bShouldAffectInputMode"))
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::NoCapture;
};
