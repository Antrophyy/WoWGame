#include "WcPrimaryGameLayout.h"
#include "UINativeTags.h"

void UWcPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterLayer(TAG_UI_LAYER_MENUSCREEN, MenuScreenStack);
	RegisterLayer(TAG_UI_LAYER_MODAL, ModalStack);
	RegisterLayer(TAG_UI_LAYER_GAME, GameStack);
	RegisterLayer(TAG_UI_LAYER_ERROR, ErrorStack);
}
