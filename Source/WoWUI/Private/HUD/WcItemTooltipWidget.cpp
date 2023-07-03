#include "HUD/WcItemTooltipWidget.h"

#include "Foundation/PalleteWidgets/WcTextBlock.h"
#include "Items/EWcItemRarity.h"
#include "Items/WcItemBase.h"

void UWcItemTooltipWidget::InitializeTooltipInformation(const UWcItemBase* Item) const
{
	// TODO(Jan.Vlcek): Make the colors accessible through the editor so that designers can choose and modify the colors themselves. Placeholder for now.
	FColor NameColor;

	switch (Item->Rarity)
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
		case EWcItemRarity::Rare:
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
	Name_Label->SetText(Item->Name);
	BindType_Label->SetText(UEnum::GetDisplayValueAsText(Item->ItemBinding));
	LevelRequirement_Label->SetText(FText::FromString(FString::FromInt(Item->LevelRequirement)));
}
