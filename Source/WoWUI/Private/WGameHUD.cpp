#include "WGameHUD.h"
#include "EazyPrimaryGameLayout.h"
#include "WoWUITags.h"
#include "Game/HUD/Modals/PlayerInventory/WItemTooltipWidget.h"

void AWGameHUD::BeginPlay()
{
	Super::BeginPlay();

	FPlatformProcess::Sleep(5.0f);
	if (UEazyPrimaryGameLayout* RootLayout = UEazyPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(WoWUITags::Layer::Game, true, HUDScreen_Class);
	}

	// Create just one item tooltip and keep re-using it instead of creating a new tooltip widget every time we mouse over an item.
	CachedItemTooltip_Widget = CreateWidget<UWItemTooltipWidget>(GetOwningPlayerController(), ItemTooltip_Class);
}
