#include "Frontend/MainMenu/WMainMenuScreen.h"

#include "EazyPrimaryGameLayout.h"
#include "WoWUITags.h"
#include "Foundation/WLabeledButton.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UWMainMenuScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayGame_Button->OnClicked().AddUObject(this, &ThisClass::HandlePlayGameButtonClicked);
	Settings_Button->OnClicked().AddUObject(this, &ThisClass::HandleSettingsButtonClicked);
	Credits_Button->OnClicked().AddUObject(this, &ThisClass::HandleCreditsButtonClicked);
	QuitGame_Button->OnClicked().AddUObject(this, &ThisClass::HandleQuitGameButtonClicked);
}

void UWMainMenuScreen::HandlePlayGameButtonClicked() const
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameMap, true);
}

void UWMainMenuScreen::HandleSettingsButtonClicked() const
{
	if (UEazyPrimaryGameLayout* PrimaryGameLayout = UEazyPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayer()))
	{
		PrimaryGameLayout->PushWidgetToLayerAsync(WoWUITags::Layer::MenuScreen, true, SettingsScreen_Class);
	}
}

void UWMainMenuScreen::HandleCreditsButtonClicked() const
{
	if (UEazyPrimaryGameLayout* PrimaryGameLayout = UEazyPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayer()))
	{
		PrimaryGameLayout->PushWidgetToLayerAsync(WoWUITags::Layer::MenuScreen, true, CreditsScreen_Class);
	}
}

void UWMainMenuScreen::HandleQuitGameButtonClicked() const
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
