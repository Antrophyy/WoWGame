#include "WPrimaryGameLayout.h"
#include "WoWUITags.h"

void UWPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterLayer(WoWUITags::Layer::Game, Game_Stack);
	RegisterLayer(WoWUITags::Layer::GameWindow, GameWindow_Stack);
	RegisterLayer(WoWUITags::Layer::MenuScreen, MenuScreen_Stack);
	RegisterLayer(WoWUITags::Layer::Modal, Modal_Stack);
}
