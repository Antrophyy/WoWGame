#include "GameLoadingScreenManager.h"
#include "HAL/ThreadHeartBeat.h"
#include "GameFramework/GameStateBase.h"
#include "IGameLoadingProcess.h"
#include "MoviePlayer.h"
#include "PreLoadScreenManager.h"
#include "ShaderPipelineCache.h"
#include "GameLoadingScreenData.h"
#include "GameLoadingScreenInputPreProcessor.h"
#include "GameLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Texture2D.h"
#include "Engine/UserInterfaceSettings.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SThrobber.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameLoadingScreen, Log, All);

DEFINE_LOG_CATEGORY(LogGameLoadingScreen);

static bool LogLoadingScreenReasonEveryFrame = false;
static FAutoConsoleVariableRef CVarLogLoadingScreenReasonEveryFrame(
	TEXT("LoadingScreen.LogLoadingScreenReasonEveryFrame"),
	LogLoadingScreenReasonEveryFrame,
	TEXT("When true, the reason the loading screen is shown or hidden will be printed to the log every frame."),
	ECVF_Default);

void UGameLoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	const UGameLoadingScreenSettings* Settings = GetDefault<UGameLoadingScreenSettings>();
	const TSubclassOf<UGameLoadingScreenData> LoadingScreenDataClass = Settings->LoadingScreenDataClass.TryLoadClass<UGameLoadingScreenData>();
	LoadingScreenData = LoadingScreenDataClass.GetDefaultObject();

	AsyncLoadAllBackgroundImages();
}

void UGameLoadingScreenManager::Deinitialize()
{
	HideLoadingScreen();

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

UGameLoadingScreenManager* UGameLoadingScreenManager::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}
	
	if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UGameLoadingScreenManager>();
	}

	return nullptr;
}

bool UGameLoadingScreenManager::ShouldCreateSubsystem(UObject* Outer) const
{
	// Only create loading screen subsystem if a movie player is enabled.
	// In other words, when we're not in the editor, we're not on the server or commandlet
	// or the game is run with -NoLoadingScreen parameter. 
	return IsMoviePlayerEnabled();
}

void UGameLoadingScreenManager::Tick(const float DeltaTime)
{
	UpdateLoadingScreen();

	TimeUntilNextLogHeartbeatSeconds = FMath::Max(TimeUntilNextLogHeartbeatSeconds - DeltaTime, 0.0);
}

void UGameLoadingScreenManager::RegisterLoadingProcessor(const TScriptInterface<IGameLoadingProcess> Interface)
{
	ExternalLoadingProcessors.Add(Interface.GetObject());
}

void UGameLoadingScreenManager::UnregisterLoadingProcessor(const TScriptInterface<IGameLoadingProcess> Interface)
{
	ExternalLoadingProcessors.Remove(Interface.GetObject());
}

UTexture2D* UGameLoadingScreenManager::GetRandomBackgroundImage()
{
	if (LoadedBackgroundImages.IsEmpty())
	{
		return nullptr;
	}

	const int32 RandomImageIndex = FMath::RandRange(0, LoadedBackgroundImages.Num() - 1);

	return LoadedBackgroundImages[RandomImageIndex];
}

FText UGameLoadingScreenManager::GetRandomTipOrHint() const
{
	if (GetLoadingScreenData()->Tips.IsEmpty())
	{
		UE_LOG(LogGameLoadingScreen, Error, TEXT("[%s::%hs] -> Loading Screen Tips array is empty. Populate it in loading screen data blueprint."), *StaticClass()->GetName(), __func__);
		return FText::GetEmpty();
	}

	const int32 RandomTipIndex = FMath::RandRange(0, GetLoadingScreenData()->Tips.Num() - 1);
	const FText RandomTip = GetLoadingScreenData()->Tips[RandomTipIndex].Tip;

	UE_CLOG(RandomTip.IsEmptyOrWhitespace(), LogGameLoadingScreen, Error, TEXT("[%s::%hs] -> Check tips array in BP_LoadingScreenData with index %d. It is either empty or it only contains a whitespace."), *StaticClass()->GetName(), __func__, RandomTipIndex);

	return RandomTip;
}

void UGameLoadingScreenManager::StartLoadingScreenOnMainThread() const
{
	if (const UGameInstance* LocalGameInstance = GetGameInstance())
	{
		// Add to the viewport at a high ZOrder to make sure it is on top of most things
		UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), 100000);
	}
}

void UGameLoadingScreenManager::StartLoadingScreenOnSlateThread() const
{
	FLoadingScreenAttributes Attributes;
	Attributes.bWaitForManualStop = false;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bAllowEngineTick = false;
	Attributes.bAutoCompleteWhenLoadingCompletes = true;
	Attributes.WidgetLoadingScreen = LoadingScreenWidget;

	if (IGameMoviePlayer* MoviePlayer = GetMoviePlayer())
	{
		if (const UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
		{
			FVector2D ClientViewportSize;
			GameViewportClient->GetViewportSize(OUT ClientViewportSize);
			const float DesiredScale = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ClientViewportSize.X, ClientViewportSize.Y));
					
			MoviePlayer->SetViewportDPIScale(DesiredScale);
		}

		MoviePlayer->SetupLoadingScreen(Attributes);
	}
}

void UGameLoadingScreenManager::StopLoadingScreenOnMainThread()
{
	const UGameInstance* LocalGameInstance = GetGameInstance();
	if (IsValid(LocalGameInstance))
	{
		if (UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient())
		{
			if (LoadingScreenWidget.IsValid())
			{
				GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
			}
		}
	}

	LoadingScreenWidget.Reset();
}

TSharedPtr<SWidget> UGameLoadingScreenManager::GetLoadingScreenWidget()
{
	return SNew(SThrobber);
}

void UGameLoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance())
	{
		return;
	}

	bCurrentlyInLoadMap = true;

	MapToBeLoaded = MapName;

	SetDebugLoadingScreenReason(TEXT("Loading a map."));

	ShowLoadingScreen();
}

void UGameLoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
	if (World != nullptr && World->GetGameInstance() == GetGameInstance())
	{
		MapToBeLoaded = TEXT("");
		bCurrentlyInLoadMap = false;
		bWaitForTextureStreaming = true;
		TimeLoadingScreenLastDismissed = FPlatformTime::Seconds();
	}
}

void UGameLoadingScreenManager::UpdateLoadingScreen()
{
	bool bLogLoadingScreenStatus = LogLoadingScreenReasonEveryFrame;

	if (ShouldShowLoadingScreen())
	{
		const UGameLoadingScreenSettings* Settings = GetDefault<UGameLoadingScreenSettings>();

		// If we don't make it to the specified checkpoint in the given time will trigger the hang detector so we can better determine where progress stalled.
		FThreadHeartBeat::Get().MonitorCheckpointStart(GetFName(), Settings->LoadingScreenHeartbeatHangDuration);

		ShowLoadingScreen();

		if (Settings->LogLoadingScreenHeartbeatInterval > 0.0f && TimeUntilNextLogHeartbeatSeconds <= 0.0)
		{
			bLogLoadingScreenStatus = true;
			TimeUntilNextLogHeartbeatSeconds = Settings->LogLoadingScreenHeartbeatInterval;
		}
	}
	else
	{
		HideLoadingScreen();

		FThreadHeartBeat::Get().MonitorCheckpointEnd(GetFName());
	}

	if (bLogLoadingScreenStatus)
	{
		UE_LOG(LogGameLoadingScreen, Log, TEXT("Loading screen showing: %d. Reason: %s"), bCurrentlyShowingLoadingScreen ? 1 : 0, *DebugLoadingScreenReason);
	}
}

bool UGameLoadingScreenManager::CheckForAnyNeedToShowLoadingScreen()
{
	// Start out with 'unknown' reason in case someone forgets to put a reason when changing this in the future.
	DebugLoadingScreenReason = TEXT("Reason for Showing/Hiding LoadingScreen is unknown!");

	const UGameInstance* LocalGameInstance = GetGameInstance();

	const FWorldContext* Context = LocalGameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		// We don't have a world context right now... better show a loading screen
		SetDebugLoadingScreenReason(TEXT("The game instance has a null WorldContext."));
		return true;
	}

	UWorld* World = Context->World();
	if (World == nullptr)
	{
		SetDebugLoadingScreenReason(TEXT("We have no world."));
		return true;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
	{
		// The game state has not yet replicated.
		SetDebugLoadingScreenReason(TEXT("GameState hasn't yet replicated."));
		return true;
	}

	if (!Context->TravelURL.IsEmpty())
	{
		// Show a loading screen when pending travel
		SetDebugLoadingScreenReason(TEXT("We have a pending travel (the TravelURL is not empty)."));
		return true;
	}

	if (Context->PendingNetGame != nullptr)
	{
		// Connecting to another server
		SetDebugLoadingScreenReason(TEXT("We are connecting to another server."));
		return true;
	}

	if (!World->HasBegunPlay())
	{
		SetDebugLoadingScreenReason(TEXT("World hasn't begun play"));
		return true;
	}

	if (World->IsInSeamlessTravel())
	{
		// Show a loading screen during seamless travel
		SetDebugLoadingScreenReason(TEXT("We are in seamless travel"));
		return true;
	}

	FString LoadingScreenReason;
	// Ask the game state if it needs a loading screen	
	if (IGameLoadingProcess::ShouldShowLoadingScreen(GameState, LoadingScreenReason))
	{
		SetDebugLoadingScreenReason(LoadingScreenReason);
		return true;
	}

	// Ask any game state components if they need a loading screen
	for (UActorComponent* TestComponent : GameState->GetComponents())
	{
		if (IGameLoadingProcess::ShouldShowLoadingScreen(TestComponent, LoadingScreenReason))
		{
			SetDebugLoadingScreenReason(LoadingScreenReason);
			return true;
		}
	}

	// Ask any of the external loading processors that may have been registered. These might be actors or components
	// that were registered by game code to tell us to keep the loading screen up while perhaps something finishes
	// streaming in.
	for (const TWeakInterfacePtr<IGameLoadingProcess>& Processor : ExternalLoadingProcessors)
	{
		if (IGameLoadingProcess::ShouldShowLoadingScreen(Processor.GetObject(), LoadingScreenReason))
		{
			SetDebugLoadingScreenReason(LoadingScreenReason);
			return true;
		}
	}

	DebugLoadingScreenReason = TEXT("Nothing wants to show it anymore.");

	return false;
}

bool UGameLoadingScreenManager::ShouldShowLoadingScreen()
{
	// Check debugging commands that force the state one way or another
#if !UE_BUILD_SHIPPING
	static bool bCmdLineNoLoadingScreen = FParse::Param(FCommandLine::Get(), TEXT("NoLoadingScreen"));
	if (bCmdLineNoLoadingScreen)
	{
		DebugLoadingScreenReason = FString(TEXT("CommandLine has 'NoLoadingScreen'"));
		return false;
	}
#endif

	// Check for a need to show the loading screen
	const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();

	// Keep the loading screen up a bit longer if desired
	bool bWantToForceShowLoadingScreen = false;
	if (bNeedToShowLoadingScreen)
	{
		// Still need to show it
		TimeLoadingScreenLastDismissed = -1.0;
	}
	else
	{
		// Don't *need* to show the screen anymore, but might still want to for a bit
		const double CurrentTime = FPlatformTime::Seconds();
		const bool bCanHoldLoadingScreen = !GIsEditor;

		const UGameLoadingScreenSettings* Settings = GetDefault<UGameLoadingScreenSettings>();
		const double HoldLoadingScreenAdditionalSecs = bCanHoldLoadingScreen ? Settings->HoldLoadingScreenAdditionalSecs : 0.0;

		if (TimeLoadingScreenLastDismissed < 0.0)
		{
			TimeLoadingScreenLastDismissed = CurrentTime;
		}
		const double TimeSinceScreenDismissed = CurrentTime - TimeLoadingScreenLastDismissed;

		// hold for an extra X seconds, to cover up streaming
		if ((HoldLoadingScreenAdditionalSecs > 0.0) && (TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs))
		{
			if (const UGameInstance* GameInstance = GetGameInstance())
			{
				// Make sure we're rendering the world at this point, so that textures will actually stream in
				//@TODO: If bNeedToShowLoadingScreen bounces back true during this window, we won't turn this off again...
				if (UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient())
				{
					GameViewportClient->bDisableWorldRendering = false;
				}
			}

			SetDebugLoadingScreenReason(TEXT("Textures are being streamed in."));
			bWantToForceShowLoadingScreen = true;
		}
		else
		{
			bWaitForTextureStreaming = false;
		}
	}

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

void UGameLoadingScreenManager::ShowLoadingScreen()
{
	if (!bCurrentlyShowingLoadingScreen)
	{
		// Unable to show loading screen if the engine is still loading with its loading screen.
		if (FPreLoadScreenManager::Get() && FPreLoadScreenManager::Get()->HasActivePreLoadScreenType(EPreLoadScreenTypes::EngineLoadingScreen))
		{
			return;
		}

		bGlobalInvalidationEnabled = GSlateEnableGlobalInvalidation;

		if (!LoadingScreenWidget.IsValid())
		{
			LoadingScreenWidget = GetLoadingScreenWidget();
		}

		StartBlockingInput();

		if (bCurrentlyInLoadMap)
		{
			// We're in a load map, the loading screen has to be on slate thread to allow animations.
			StartLoadingScreenOnSlateThread();
		}
		else
		{
			// We're not blocking main thread, we can show screen on the MT.
			StartLoadingScreenOnMainThread();
			bCurrentlyShowingLoadingScreen = true;
		}
		
		OnLoadingScreenStarted();
		
		LoadingScreenVisibilityChanged.Broadcast(true);
	}

	// Make sure we only manipulate with performance settings on game thread instead of render thread.
	if (!bFastLoadingEnabled && IsInGameThread())
	{
		ChangePerformanceSettings(true);

		if (bGlobalInvalidationEnabled)
		{
			FSlateApplication::Get().ToggleGlobalInvalidation(false);
		}
	}
}

void UGameLoadingScreenManager::HideLoadingScreen()
{
	// It may just happen that the application has been killed and there is no window anymore. No need to clean up anything anymore
	if (!IsValid(GEngine))
	{
		return;
	}
	
	if (bCurrentlyShowingLoadingScreen)
	{
		UE_LOG(LogGameLoadingScreen, Log, TEXT("Garbage Collecting before dropping loading screen."));
		GEngine->ForceGarbageCollection(true);

		StopBlockingInput();

		StopLoadingScreenOnMainThread();

		bCurrentlyShowingLoadingScreen = false;

		// Let observers know that the loading screen is done
		LoadingScreenVisibilityChanged.Broadcast(false);

		OnLoadingScreenStopped();

		const double LoadingScreenDuration = FPlatformTime::Seconds() - TimeLoadingScreenShown;
		UE_LOG(LogGameLoadingScreen, Log, TEXT("LoadingScreen was visible for %.2fs"), LoadingScreenDuration);
	}

	// Make sure we only manipulate with performance settings on a game thread instead of render thread.
	if (bFastLoadingEnabled && IsInGameThread() && FSlateApplication::IsInitialized())
	{
		if (bGlobalInvalidationEnabled)
		{
			FSlateApplication::Get().ToggleGlobalInvalidation(true);
		}

		ChangePerformanceSettings(false);
	}
}

void UGameLoadingScreenManager::ChangePerformanceSettings(const bool bEnablingLoadingScreen)
{
	if (bEnablingLoadingScreen == bFastLoadingEnabled)
	{
		return;
	}

	bFastLoadingEnabled = bEnablingLoadingScreen;

	FShaderPipelineCache::SetBatchMode(bEnablingLoadingScreen ? FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	if (UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
	{
		// Don't bother drawing the 3D world while we're loading
		GameViewportClient->bDisableWorldRendering = bEnablingLoadingScreen;

		// Make sure to prioritize streaming in levels if the loading screen is up
		if (const UWorld* ViewportWorld = GameViewportClient->GetWorld())
		{
			if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
			{
				WorldSettings->bHighPriorityLoadingLocal = bEnablingLoadingScreen;
			}
		}
	}

	if (bEnablingLoadingScreen)
	{
		// Set a new hang detector timeout multiplier when the loading screen is visible.
		double HangDurationMultiplier;
		if (!GConfig || !GConfig->GetDouble(TEXT("Core.System"), TEXT("LoadingScreenHangDurationMultiplier"), HangDurationMultiplier, GEngineIni))
		{
			HangDurationMultiplier = 1.0;
		}
		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);

		// Do not report hitches while the loading screen is up
		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}
	else
	{
		// Restore the hang detector timeout when we hide the loading screen
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);

		// Resume reporting hitches now that the loading screen is down
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}

void UGameLoadingScreenManager::SetDebugLoadingScreenReason(const FString& Reason)
{
	if (!Reason.Equals(DebugLoadingScreenReason))
	{
		DebugLoadingScreenReason = Reason;
		LoadingScreenReasonChanged.Broadcast(DebugLoadingScreenReason);
	}
}

void UGameLoadingScreenManager::AsyncLoadAllBackgroundImages()
{
	if (!IsValid(LoadingScreenData))
	{
		UE_LOG(LogGameLoadingScreen, Error, TEXT("[%s::%hs] -> Could not get Loading Screen data."), *StaticClass()->GetName(), __func__);
		return;
	}

	TArray<FGameLoadingScreenImage> BackgroundImages;
	GetLoadingScreenData()->GameplayTagTextureMap.GenerateValueArray(BackgroundImages);

	TArray<TSoftObjectPtr<UTexture2D>> ImageTextures;
	for (FGameLoadingScreenImage BackgroundImageArrays : BackgroundImages)
	{
		ImageTextures.Append(BackgroundImageArrays.ImageTextures);
	}

	if (ImageTextures.IsEmpty())
	{
		return;
	}

	// We will randomly select one of the loading screen and load it synchronously just so that we have it ready for the first loading screen.
	// All the other loading screens will be loaded asynchronously.
	const int32 RandomImageIndex = FMath::RandRange(0, ImageTextures.Num() - 1);

	UAssetManager::Get().GetStreamableManager().RequestSyncLoad(ImageTextures[RandomImageIndex].ToSoftObjectPath());
	ensureAlwaysMsgf(ImageTextures[RandomImageIndex].Get(), TEXT("Failed to load %s"), *ImageTextures[RandomImageIndex].ToSoftObjectPath().ToString());
	
	LoadedBackgroundImages.Emplace(ImageTextures[RandomImageIndex].Get());

	for (int32 i = 0; i < ImageTextures.Num() - 1; ++i)
	{
		if (RandomImageIndex == i)
		{
			continue;
		}

		TSoftObjectPtr<UTexture2D> ImageTexture = ImageTextures[i];

		UAssetManager::GetStreamableManager().RequestAsyncLoad(ImageTexture.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, ImageTexture]
		{
			if (ImageTexture.Get())
			{
				LoadedBackgroundImages.Emplace(ImageTexture.Get());
			}
		}));
	}
}

void UGameLoadingScreenManager::StartBlockingInput()
{
	if (!InputPreProcessor.IsValid())
	{
		InputPreProcessor = MakeShareable<FGameLoadingScreenInputPreProcessor>(new FGameLoadingScreenInputPreProcessor());
		FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
	}
}

void UGameLoadingScreenManager::StopBlockingInput()
{
	if (InputPreProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
		InputPreProcessor.Reset();
	}
}
