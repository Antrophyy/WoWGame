#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UObject/SoftObjectPath.h"
#include "GameLoadingScreenSettings.generated.h"

class UGameLoadingScreenData;

// Settings for a loading screen system.
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Game Loading Screen"), ClassGroup=UI)
class GAMELOADINGSCREEN_API UGameLoadingScreenSettings final : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:

	UGameLoadingScreenSettings();

	// The widget to load for the loading screen.
	UPROPERTY(config, EditAnywhere, Category=Display, meta=(MetaClass="/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;
	
	// How long to hold the loading screen up after other loading finishes (in seconds) to
	// give texture streaming a chance to avoid blurriness.
	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float HoldAdditionalTime = 2.0f;

	// The interval in seconds beyond which the loading screen is considered permanently hung (if non-zero).
	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float LoadingScreenHeartbeatHangDuration = 0.0f;

	// The interval in seconds between each log of what is keeping a loading screen up (if non-zero).
	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float LogLoadingScreenHeartbeatInterval = 5.0f;

	// When true, the reason the loading screen is shown or hidden will be printed to the log every frame.
	UPROPERTY(Transient, EditAnywhere, Category=Debugging, meta=(ConsoleVariable="LoadingScreen.LogLoadingScreenReasonEveryFrame"))
	bool LogLoadingScreenReasonEveryFrame = false;
};
