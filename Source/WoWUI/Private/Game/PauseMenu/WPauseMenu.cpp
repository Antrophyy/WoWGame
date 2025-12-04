#include "Game/PauseMenu/WPauseMenu.h"

#include "Foundation/WLabeledButton.h"
#include "Kismet/KismetSystemLibrary.h"

void UWPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Resume_Button->OnClicked().AddUObject(this, &ThisClass::HandleBackAction);
	Settings_Button->OnClicked().AddUObject(this, &ThisClass::HandleSettingsButtonClicked);
	QuitGame_Button->OnClicked().AddUObject(this, &ThisClass::HandleQuitGameButtonClicked);
	
	Settings_Button->SetIsInteractionEnabled(false);
	Settings_Button->DisableButtonWithReason(FText::FromString(TEXT("Not yet implemented.")));
}

void UWPauseMenu::HandleSettingsButtonClicked() const
{
	// TODO(Jan.Vlcek): Add implementation. 
}

void UWPauseMenu::HandleQuitGameButtonClicked() const
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
