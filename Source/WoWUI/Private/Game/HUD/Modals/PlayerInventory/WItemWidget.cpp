#include "Game/HUD/Modals/PlayerInventory/WItemWidget.h"

#include "WGameHUD.h"
#include "Components/Image.h"
#include "Game/HUD/Modals/PlayerInventory/WItemTooltipWidget.h"

void UWItemWidget::SetItemDetails(const FWcItemDetail& InItemDetails)
{
	ItemDetails = InItemDetails;
}

void UWItemWidget::SetItemIcon(UTexture2D* Texture) const
{
	Icon_Image->SetBrushFromTexture(Texture, true);
}

void UWItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (IsValid(GetHUD()))
	{
		return;
	}
	
	if (UWItemTooltipWidget* TooltipWidget = GetHUD<AWGameHUD>()->GetCachedTooltipWidget())
	{
		TooltipWidget->UpdateTooltipInformation(ItemDetails);
		SetToolTip(TooltipWidget);
	}
}

void UWItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// Have to reset the tooltip widget as SGI doesn't like when one tooltip widget is used by multiple widgets.
	SetToolTip(nullptr);
}
