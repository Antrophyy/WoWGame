#include "Frontend/WcMainMenuScreen.h"
#include "Foundation/WcButtonBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UWcMainMenuScreen::NativeOnInitialized()
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

void UWcMainMenuScreen::HandlePlayGameButtonClicked()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameMap, true);
}

void UWcMainMenuScreen::HandleSettingsButtonClicked()
{
	// #TODO: Add
}

void UWcMainMenuScreen::HandleCreditsButtonClicked()
{
	// #TODO: Add
}

void UWcMainMenuScreen::HandleQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
