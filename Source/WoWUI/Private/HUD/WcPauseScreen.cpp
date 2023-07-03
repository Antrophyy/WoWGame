#include "HUD/WcPauseScreen.h"
#include "Foundation/WcButtonBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UWcPauseScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Resume_Button->OnClicked().AddUObject(this, &ThisClass::HandleBackAction);

	Settings_Button->OnClicked().AddUObject(this, &ThisClass::HandleSettingsButtonClicked);
	Settings_Button->SetIsInteractionEnabled(false);
	Settings_Button->DisableButtonWithReason(FText::FromString(TEXT("Not yet implemented.")));

	QuitGame_Button->OnClicked().AddUObject(this, &ThisClass::HandleQuitGameButtonClicked);
}

void UWcPauseScreen::HandleSettingsButtonClicked()
{
	// TODO(Jan.Vlcek): Add implementation. 
}

void UWcPauseScreen::HandleQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
