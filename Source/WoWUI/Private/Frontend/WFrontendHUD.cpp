#include "Frontend/WFrontendHUD.h"

#include "RarePrimaryGameLayout.h"
#include "UINativeTags.h"

void AWFrontendHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (URarePrimaryGameLayout* RootLayout = URarePrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_MENUSCREEN, true, MainMenuScreen_Class);
	}
}
