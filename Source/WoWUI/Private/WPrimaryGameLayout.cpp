#include "WPrimaryGameLayout.h"
#include "UINativeTags.h"

void UWPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterLayer(TAG_UI_LAYER_GAME, Game_Stack);
	RegisterLayer(TAG_UI_LAYER_GAMEWINDOW, GameWindow_Stack);
	RegisterLayer(TAG_UI_LAYER_MENUSCREEN, MenuScreen_Stack);
	RegisterLayer(TAG_UI_LAYER_MODAL, Modal_Stack);
	RegisterLayer(TAG_UI_LAYER_ERROR, Error_Stack);
}
