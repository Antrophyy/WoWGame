// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Engine/DeveloperSettings.h"
#include "RareUISettings.generated.h"

UCLASS(Config = Engine, DisplayName = "Rare UI Settings", ClassGroup=UI)
class RAREUI_API URareUISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URareUISettings();

	UPROPERTY(Config, EditAnywhere, Category = "Layout")
	int32 ViewportLayoutDefaultZOrder = 1000;

	// If enabled, will fire an ensure anytime 'AddToViewport' is called during gameplay. Useful to prevent unexpected behavior. You can make exceptions for class types in UIPolicy BP
	UPROPERTY(Config, EditAnywhere, Category = "Ensures|AddToViewport")
	bool bEnableAddToViewportEnsures = false;

	// These classes will not trigger an ensure when AddToViewport is called on them. Should only be used for a few exceptions.
	UPROPERTY(Config, EditAnywhere, Category = "Ensures|AddToViewport", meta = (EditCondition = "bEnableAddToViewportEnsures", EditConditionHides, AllowAbstract = "true"))
	TArray<TObjectPtr<UClass>> ClassTypesToIgnoreForAddToViewport;
};
