#include "WcGameHUD.h"
#include "PrimaryGameLayout.h"
#include "UINativeTags.h"
#include "HUD/WcItemTooltipWidget.h"

void AWcGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayerController()))
	{
		RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_GAME, true, HUDWidget_Class);
	}

	// Create just one item tooltip and keep re-using it instead of creating a new tooltip widget every we mouse over an item.
	CachedItemTooltip_Widget = CreateWidget<UWcItemTooltipWidget>(GetOwningPlayerController(), ItemTooltip_Class);
}
