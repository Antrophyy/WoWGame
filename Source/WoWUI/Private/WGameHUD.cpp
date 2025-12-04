#include "WGameHUD.h"
#include "RarePrimaryGameLayout.h"
#include "UINativeTags.h"
#include "Game/HUD/Modals/PlayerInventory/WItemTooltipWidget.h"

void AWGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (URarePrimaryGameLayout* RootLayout = URarePrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_GAME, true, HUDScreen_Class);
	}

	// Create just one item tooltip and keep re-using it instead of creating a new tooltip widget every time we mouse over an item.
	CachedItemTooltip_Widget = CreateWidget<UWItemTooltipWidget>(GetOwningPlayerController(), ItemTooltip_Class);
}
