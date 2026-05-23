#include "Frontend/WFrontendHUD.h"

#include "RarePrimaryGameLayout.h"
#include "WoWUITags.h"

void AWFrontendHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (URarePrimaryGameLayout* RootLayout = URarePrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(WoWUITags::Layer::MenuScreen, true, MainMenuScreen_Class);
	}
}
