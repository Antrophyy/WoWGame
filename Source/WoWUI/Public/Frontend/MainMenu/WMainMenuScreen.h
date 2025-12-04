#pragma once

#include "Foundation/WActivatableWidget.h"
#include "WMainMenuScreen.generated.h"

class UWLabeledButton;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWMainMenuScreen final : public UWActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly, meta=(Validate))
	TSoftObjectPtr<UWorld> GameMap;

private: // Delegate Handling

	void HandlePlayGameButtonClicked();
	void HandleSettingsButtonClicked();
	void HandleCreditsButtonClicked();
	void HandleQuitGameButtonClicked();

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> PlayGame_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> Settings_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> Credits_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWLabeledButton> QuitGame_Button;
};
