#pragma once

#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"
#include "EazyUISettings.generated.h"

class UEazyGameUIPolicy;

UCLASS(Config = Engine, DisplayName = "EazyUI Settings", ClassGroup=UI)
class EAZYUI_API UEazyUISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UEazyUISettings();

	UPROPERTY(Config, EditAnywhere, Category = "Layout")
	int32 ViewportLayoutDefaultZOrder = 1000;
	
	UPROPERTY(Config, EditAnywhere, Category = "Layout")
	TSoftClassPtr<UEazyGameUIPolicy> DefaultUIPolicy;

	// If enabled, will fire an ensure anytime 'AddToViewport' is called during gameplay. Useful to prevent unexpected behavior. You can make exceptions for class types in UIPolicy BP
	UPROPERTY(Config, EditAnywhere, Category = "Ensures|AddToViewport")
	bool bEnableAddToViewportEnsures = false;

	// These classes will not trigger an ensure when AddToViewport is called on them. Should only be used for a few exceptions.
	UPROPERTY(Config, EditAnywhere, Category = "Ensures|AddToViewport", meta = (EditCondition = "bEnableAddToViewportEnsures", EditConditionHides, AllowAbstract = "true"))
	TArray<TObjectPtr<UClass>> ClassTypesToIgnoreForAddToViewport;
};
