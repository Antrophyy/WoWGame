#include "Game/HUD/Modals/PlayerInventory/WItemTooltipWidget.h"

#include "Components/TextBlock.h"
#include "Items/EWcItemRarity.h"
#include "Items/WcItemDetail.h"

void UWItemTooltipWidget::UpdateTooltipInformation(const FWcItemDetail& Item) const
{
	// TODO(Jan.Vlcek): Make the colors accessible through the editor so that designers can choose and modify the colors themselves. Placeholder for now.
	FColor NameColor;

	switch (Item.Rarity)
	{
		case EWcItemRarity::Poor:
			NameColor = FColor(157, 157, 157);
			break;
		case EWcItemRarity::Common:
			NameColor = FColor::White;
			break;
		case EWcItemRarity::Uncommon:
			NameColor = FColor(30, 255, 0);
			break;
		case EWcItemRarity::Ez:
			NameColor = FColor(0, 112, 221);
			break;
		case EWcItemRarity::Epic:
			NameColor = FColor(163, 53, 238);
			break;
		case EWcItemRarity::Legendary:
			NameColor = FColor(255, 128, 0);
			break;
		case EWcItemRarity::Artifact:
			NameColor = FColor(230, 204, 128);
			break;
		case EWcItemRarity::Heirloom:
			NameColor = FColor(0, 204, 255);
			break;
		default:
			NameColor = FColor::White;
			break;
	}
	
	Name_Label->SetColorAndOpacity(NameColor);
	Name_Label->SetText(Item.Name);
	BindType_Label->SetText(GetBindTypeText(Item.Binding));
	LevelRequirement_Label->SetText(FText::FromString(FString::FromInt(Item.LevelRequirement)));
}

const FText& UWItemTooltipWidget::GetBindTypeText(const EWcItemBinding Binding) const
{
	static const FText BindOnPickupText = FText::FromString("Bind on Pickup");
	static const FText BindOnAcquireText = FText::FromString("Bind on Acquire");
	static const FText BindOnUseText = FText::FromString("Bind on Use");
	static const FText BindToAccountText = FText::FromString("Bind to Account");
	
	switch (Binding)
	{
		case EWcItemBinding::BindOnPickup:
			return BindOnPickupText;
		case EWcItemBinding::BindOnAcquire:
			return BindOnAcquireText;
		case EWcItemBinding::BindOnUse:
			return BindOnUseText;
		case EWcItemBinding::BindToAccount:
			return BindToAccountText;
		default:
			return FText::GetEmpty();
	}
}
