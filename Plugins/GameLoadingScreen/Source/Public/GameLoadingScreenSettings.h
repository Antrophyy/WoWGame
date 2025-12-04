#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "GameLoadingScreenSettings.generated.h"

class UGameLoadingScreenData;

// Settings for a loading screen system.
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Game Loading Screen"), ClassGroup=UI)
class GAMELOADINGSCREEN_API UGameLoadingScreenSettings final : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:

	UGameLoadingScreenSettings();

	// This class contains loading screen tips & loading screen images that the manager will use to display.
	UPROPERTY(config, EditAnywhere, Category=Display, meta=(MetaClass="/Script/GameLoadingScreen.GameLoadingScreenData"))
	FSoftClassPath LoadingScreenDataClass;

	// How long to hold the loading screen up after other loading finishes (in seconds) to
	// try to give texture streaming a chance to avoid blurriness
	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float HoldLoadingScreenAdditionalSecs = 2.0f;

	// The interval in seconds beyond which the loading screen is considered permanently hung (if non-zero).
	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float LoadingScreenHeartbeatHangDuration = 0.0f;

	// The interval in seconds between each log of what is keeping a loading screen up (if non-zero).
	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float LogLoadingScreenHeartbeatInterval = 5.0f;

	// When true, the reason the loading screen is shown or hidden will be printed to the log every frame.
	UPROPERTY(Transient, EditAnywhere, Category=Debugging, meta=(ConsoleVariable="GameLoadingScreen.LogLoadingScreenReasonEveryFrame"))
	bool LogLoadingScreenReasonEveryFrame = false;
};
