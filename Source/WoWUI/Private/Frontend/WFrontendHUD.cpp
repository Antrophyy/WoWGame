#include "Frontend/WFrontendHUD.h"

#include "EazyPrimaryGameLayout.h"
#include "WoWUITags.h"

void AWFrontendHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEazyPrimaryGameLayout* RootLayout = UEazyPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(WoWUITags::Layer::MenuScreen, true, MainMenuScreen_Class);
	}
}
