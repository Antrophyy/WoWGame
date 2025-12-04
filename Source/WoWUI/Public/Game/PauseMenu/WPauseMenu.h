#pragma once

#include "Foundation/WActivatableWidget.h"
#include "WPauseMenu.generated.h"

class UWLabeledButton;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWPauseMenu : public UWActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private: // Internal
	
	void HandleSettingsButtonClicked() const;
	void HandleQuitGameButtonClicked() const;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> Resume_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> Settings_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> QuitGame_Button;
};
