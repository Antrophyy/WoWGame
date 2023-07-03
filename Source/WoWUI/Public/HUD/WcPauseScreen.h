#pragma once

#include "Foundation/WcActivatableWidget.h"
#include "WcPauseScreen.generated.h"

class UWcButtonBase;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcPauseScreen : public UWcActivatableWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

private:

	void HandleSettingsButtonClicked();
	
	void HandleQuitGameButtonClicked();

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> Resume_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> Settings_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcButtonBase> QuitGame_Button;
};
