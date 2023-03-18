#pragma once

#include "CommonActivatableWidget.h"
#include "ExtensionActivatableWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class UIEXTENSIONS_API UExtensionActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	/**
	 * @brief Retries to focus the Desired Focus Target specified by GetDesiredFocusTarget.
	 */
	UFUNCTION(BlueprintCallable)
	void Refocus() const;

protected:

	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditAnywhere, Category = Input)
	ECommonInputMode InputConfig = ECommonInputMode::Menu;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditAnywhere, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::NoCapture;
};
