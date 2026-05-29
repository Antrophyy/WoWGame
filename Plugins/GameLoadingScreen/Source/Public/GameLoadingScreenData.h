#pragma once

#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "GameLoadingScreenData.generated.h"

class UTexture2D;

USTRUCT(Category="UI|LoadingScreen")
struct GAMELOADINGSCREEN_API FGameLoadingScreenTip
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Miscellaneous)
	FText Tip;

	UPROPERTY(EditAnywhere, Category=Miscellaneous, meta=(Categories="UI.LoadingScreen.Tips"))
	FGameplayTagContainer FilterTags;
};

USTRUCT(Category="UI|LoadingScreen")
struct GAMELOADINGSCREEN_API FGameLoadingScreenImage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Miscellaneous)
	TArray<TSoftObjectPtr<UTexture2D>> ImageTextures;
};

UCLASS(Abstract, Blueprintable, ClassGroup=UI)
class GAMELOADINGSCREEN_API UGameLoadingScreenData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category=Miscellaneous, meta=(ShowOnlyInnerProperties, TitleProperty="Tip", ForceInlineRow))
	TArray<FGameLoadingScreenTip> Tips;

	UPROPERTY(EditAnywhere, Category=Miscellaneous, meta=(Categories="UI.LoadingScreen.Images", ForceInlineRow), DisplayName="Loading Screen Images")
	TMap<FGameplayTag, FGameLoadingScreenImage> GameplayTagTextureMap;
};
