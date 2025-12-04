#include "Frontend/MainMenu/WMainMenuScreen.h"

#include "Foundation/WLabeledButton.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UWMainMenuScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayGame_Button->OnClicked().AddUObject(this, &ThisClass::HandlePlayGameButtonClicked);
	Settings_Button->OnClicked().AddUObject(this, &ThisClass::HandleSettingsButtonClicked);
	
	Settings_Button->SetIsInteractionEnabled(false);
	Settings_Button->DisableButtonWithReason(FText::FromString(TEXT("Not yet implemented.")));
	
	Credits_Button->OnClicked().AddUObject(this, &ThisClass::HandleCreditsButtonClicked);
	Credits_Button->SetIsInteractionEnabled(false);
	Credits_Button->DisableButtonWithReason(FText::FromString(TEXT("Not yet implemented.")));
	
	QuitGame_Button->OnClicked().AddUObject(this, &ThisClass::HandleQuitGameButtonClicked);
}

void UWMainMenuScreen::HandlePlayGameButtonClicked()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameMap, true);
}

void UWMainMenuScreen::HandleSettingsButtonClicked()
{
	// #TODO: Add
}

void UWMainMenuScreen::HandleCreditsButtonClicked()
{
	// #TODO: Add
}

void UWMainMenuScreen::HandleQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
