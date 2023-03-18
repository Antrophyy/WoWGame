#include "WcGameHUD.h"
#include "HUD/WcHUDWidget.h"
#include "PrimaryGameLayout.h"
#include "UINativeTags.h"

void AWcGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_GAME, true, HUDWidgetClass);
	}
}
