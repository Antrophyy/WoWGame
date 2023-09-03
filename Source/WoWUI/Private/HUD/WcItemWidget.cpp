#include "HUD/WcItemWidget.h"

#include "WcGameHUD.h"
#include "Components/Image.h"
#include "HUD/WcItemTooltipWidget.h"

void UWcItemWidget::SetItemDetails(const FWcItemDetail& InItemDetails)
{
	ItemDetails = InItemDetails;
}

void UWcItemWidget::SetItemIcon(UTexture2D* Texture) const
{
	Icon_Image->SetBrushFromTexture(Texture, true);
}

void UWcItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	const AWcGameHUD* HUDActor = GetOwningPlayerChecked().GetHUD<AWcGameHUD>();
	if (!ensure(HUDActor))
		return;

	if (UWcItemTooltipWidget* TooltipWidget = HUDActor->GetCachedTooltipWidget())
	{
		TooltipWidget->UpdateTooltipInformation(ItemDetails);
		SetToolTip(TooltipWidget);
	}
}

void UWcItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// Have to reset the tooltip widget as SGI doesn't like when one tooltip widget is used by multiple widgets.
	SetToolTip(nullptr);
}
