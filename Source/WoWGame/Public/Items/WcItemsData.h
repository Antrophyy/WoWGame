#pragma once

#include "UObject/Object.h"
#include "WcItemsData.generated.h"

class UWcItemBase;

UCLASS(Blueprintable)
class WOWGAME_API UWcItemsData : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UWcItemBase>> Items;
};
