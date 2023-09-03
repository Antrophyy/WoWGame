#pragma once

#include "EWcItemBinding.h"
#include "EWcItemRarity.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "WcItemDetail.generated.h"

class UTexture2D;

USTRUCT(Blueprintable)
struct FWcItemDetail : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	EWcItemRarity Rarity = EWcItemRarity::Common;

	UPROPERTY(EditAnywhere)
	EWcItemBinding Binding = EWcItemBinding::BindOnAcquire;

	UPROPERTY(EditAnywhere)
	uint8 LevelRequirement = 0;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(EditAnywhere, meta=(ForceInlineRow), meta=(Categories="Character.Attributes"))
	TMap<FGameplayTag, float> Attributes;
};
