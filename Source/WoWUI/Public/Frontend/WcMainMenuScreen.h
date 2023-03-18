#pragma once

#include "Foundation/WcActivatableWidget.h"
#include "WcMainMenuScreen.generated.h"

class UWcButtonBase;
UCLASS(ClassGroup=UI)
class WOWUI_API UWcMainMenuScreen final : public UWcActivatableWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameMap;

private: // Delegate Handling

	void HandlePlayGameButtonClicked();

	void HandleSettingsButtonClicked();

	void HandleCreditsButtonClicked();

	void HandleQuitGameButtonClicked();

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> PlayGame_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> Settings_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> Credits_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> QuitGame_Button;
};
