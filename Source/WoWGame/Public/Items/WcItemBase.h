#pragma once

#include "Engine/Texture2D.h"
#include "WcItemBase.generated.h"

enum class EWcItemBinding : uint8;
enum class EWcItemRarity : uint8;

UCLASS()
class WOWGAME_API UWcItemBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere)
	FText Name;
	
	UPROPERTY(EditAnywhere)
	EWcItemRarity Rarity;

	UPROPERTY(EditAnywhere)
	EWcItemBinding ItemBinding;

	UPROPERTY(EditAnywhere, meta=(UIMin=0, ClampMin=0))
	int32 LevelRequirement = 0;
};
