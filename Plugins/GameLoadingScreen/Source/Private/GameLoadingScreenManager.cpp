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
#include "Framework/Application/SlateApplication.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameLoadingScreen, Log, All);

DEFINE_LOG_CATEGORY(LogGameLoadingScreen);

namespace LoadingScreenCVars
{
	static bool LogLoadingScreenReasonEveryFrame = false;
	static FAutoConsoleVariableRef CVarLogLoadingScreenReasonEveryFrame(
		TEXT("LoadingScreen.LogLoadingScreenReasonEveryFrame"),
		LogLoadingScreenReasonEveryFrame,
		TEXT("When true, the reason the loading screen is shown or hidden will be printed to the log every frame."),
		ECVF_Default);

	static bool ForceLoadingScreenVisible = false;
	static FAutoConsoleVariableRef CVarForceLoadingScreenVisible(
		TEXT("LoadingScreen.AlwaysShow"),
		ForceLoadingScreenVisible,
		TEXT("Force the loading screen to show."),
		ECVF_Default);
}

void UGameLoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	//Streaming pause override showing default throbber 
	BeginDelegate.BindUObject(this, &ThisClass::BeginStreamingPause);
	EndDelegate.BindUObject(this, &ThisClass::StopStreamingPause);

	check(GEngine);

	GEngine->RegisterBeginStreamingPauseRenderingDelegate(&BeginDelegate);
	GEngine->RegisterEndStreamingPauseRenderingDelegate(&EndDelegate);
}

void UGameLoadingScreenManager::Deinitialize()
{
	HideLoadingScreen();

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	BeginDelegate.Unbind();
	EndDelegate.Unbind();
	if (GEngine)
	{
		GEngine->RegisterBeginStreamingPauseRenderingDelegate(nullptr);
		GEngine->RegisterEndStreamingPauseRenderingDelegate(nullptr);
	}

	// We are done, so do not attempt to tick us again
	SetTickableTickType(ETickableTickType::Never);
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
	// Only clients have loading screens
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();
	return !bIsServerWorld;
}

void UGameLoadingScreenManager::Tick(const float DeltaTime)
{
	UpdateLoadingScreen();
	TimeUntilNextLogHeartbeatSeconds = FMath::Max(TimeUntilNextLogHeartbeatSeconds - DeltaTime, 0.0);
}

void UGameLoadingScreenManager::SlateTick(const float DeltaTime)
{
	Tick(DeltaTime);
}

ETickableTickType UGameLoadingScreenManager::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}

	return ETickableTickType::Conditional;
}

bool UGameLoadingScreenManager::IsTickable() const
{
	// Don't tick if we don't have a game viewport client, this catches cases that ShouldCreateSubsystem does not
	const UGameInstance* GameInstance = GetGameInstance();
	return GameInstance && GameInstance->GetGameViewportClient();
}

void UGameLoadingScreenManager::RegisterLoadingProcessor(const TScriptInterface<IGameLoadingProcess> Interface)
{
	ExternalLoadingProcessors.Add(Interface.GetObject());
}

void UGameLoadingScreenManager::UnregisterLoadingProcessor(const TScriptInterface<IGameLoadingProcess> Interface)
{
	ExternalLoadingProcessors.Remove(Interface.GetObject());
}

void UGameLoadingScreenManager::BeginStreamingPause(class FViewport* Viewport)
{
	// Do nothing by default - override it if needed
}

void UGameLoadingScreenManager::StopStreamingPause()
{
	// Do nothing by default - override it if needed
}

void UGameLoadingScreenManager::StartLoadingScreen()
{
	FLoadingScreenAttributes Attributes;
	Attributes.bWaitForManualStop = true;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.MinimumLoadingScreenDisplayTime = 1;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bAllowEngineTick = false;
	Attributes.PlaybackType = MT_Normal;

	const UGameLoadingScreenSettings* Settings = GetDefault<UGameLoadingScreenSettings>();
	const TSubclassOf<UUserWidget> LoadingScreenWidgetClass = Settings->LoadingScreenWidget.TryLoadClass<UUserWidget>();

	if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(), LoadingScreenWidgetClass, NAME_None))
	{
		LoadingScreenWidget = UserWidget->TakeWidget();
	}

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

		// There is no tick while the movie player is running, so we have to subscribe to render thread ticks to update the loading screen status.
		FSlateApplication::Get().OnPostTick().AddUObject(this, &ThisClass::SlateTick);

		MoviePlayer->PlayMovie();
	}
}

void UGameLoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance())
	{
		return;
	}

	bCurrentlyInLoadMap = true;

	if (IsValid(GEngine) && GEngine->IsInitialized())
	{
		UpdateLoadingScreen();
	}
}

void UGameLoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
	if (World != nullptr && World->GetGameInstance() == GetGameInstance())
	{
		bCurrentlyInLoadMap = false;
	}
}

void UGameLoadingScreenManager::UpdateLoadingScreen()
{
	bool bLogLoadingScreenStatus = LoadingScreenCVars::LogLoadingScreenReasonEveryFrame;

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
	
	if (LoadingScreenCVars::ForceLoadingScreenVisible)
	{
		SetDebugLoadingScreenReason(TEXT("CommonLoadingScreen.AlwaysShow is true"));
		return true;
	}

	const FWorldContext* Context = LocalGameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		// We don't have a world context right now... better show a loading screen
		SetDebugLoadingScreenReason(TEXT("The game instance has a null WorldContext."));
		return true;
	}

	const UWorld* World = Context->World();
	if (!IsValid(World))
	{
		SetDebugLoadingScreenReason(TEXT("We have no world."));
		return true;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (!IsValid(GameState))
	{
		// The game state has not yet replicated.
		SetDebugLoadingScreenReason(TEXT("GameState hasn't yet replicated."));
		return true;
	}

	if (bCurrentlyInLoadMap)
	{
		// Show a loading screen if we are in LoadMap
		SetDebugLoadingScreenReason(TEXT("bCurrentlyInLoadMap is true"));
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
	for (const TWeakObjectPtr<UObject> Processor : ExternalLoadingProcessors)
	{
		if (!Processor->Implements<UGameLoadingProcess>())
		{
			continue;
		}

		if (IGameLoadingProcess::ShouldShowLoadingScreen(Processor.Get(), LoadingScreenReason))
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
#endif // !UE_BUILD_SHIPPING

	// Can't show a loading screen if there's no game viewport
	const UGameInstance* LocalGameInstance = GetGameInstance();
	if (LocalGameInstance->GetGameViewportClient() == nullptr)
	{
		return false;
	}

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
		const double HoldLoadingScreenAdditionalSecs = bCanHoldLoadingScreen ? Settings->HoldAdditionalTime : 0.0;

		if (TimeLoadingScreenLastDismissed < 0.0)
		{
			TimeLoadingScreenLastDismissed = CurrentTime;
		}
		const double TimeSinceScreenDismissed = CurrentTime - TimeLoadingScreenLastDismissed;

		// hold for an extra X seconds, to cover up streaming
		if ((HoldLoadingScreenAdditionalSecs > 0.0) && (TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs))
		{
			// Make sure we're rendering the world at this point, so that textures will actually stream in
			//@TODO: If bNeedToShowLoadingScreen bounces back true during this window, we won't turn this off again...
			if (UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
			{
				GameViewportClient->bDisableWorldRendering = true;
			}

			SetDebugLoadingScreenReason(TEXT("Textures are being streamed in."));
			bWantToForceShowLoadingScreen = true;
		}
	}

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

void UGameLoadingScreenManager::ShowLoadingScreen()
{
	if (!bCurrentlyShowingLoadingScreen && !IsShowingInitialLoadingScreen())
	{
		// Unable to show loading screen if the engine is still loading with its loading screen.
		if (FPreLoadScreenManager::Get() && FPreLoadScreenManager::Get()->HasActivePreLoadScreenType(EPreLoadScreenTypes::EngineLoadingScreen))
		{
			return;
		}

		bGlobalInvalidationEnabled = GSlateEnableGlobalInvalidation;

		StartBlockingInput();
		StartLoadingScreen();
		bCurrentlyShowingLoadingScreen = true;

		TimeLoadingScreenShown = FPlatformTime::Seconds();
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

	if (IsShowingInitialLoadingScreen())
	{
		return;
	}

	if (bCurrentlyShowingLoadingScreen)
	{
		UE_LOG(LogGameLoadingScreen, Log, TEXT("Garbage Collecting before dropping loading screen."));
		GEngine->ForceGarbageCollection(true);

		GetMoviePlayer()->StopMovie();

		LoadingScreenWidget.Reset();

		bCurrentlyShowingLoadingScreen = false;

		// Let observers know that the loading screen is done
		StopBlockingInput();
		LoadingScreenVisibilityChanged.Broadcast(false);

		OnLoadingScreenStopped();

		const double LoadingScreenDuration = FPlatformTime::Seconds() - TimeLoadingScreenShown;
		UE_LOG(LogGameLoadingScreen, Log, TEXT("LoadingScreen was visible for %.2fs"), LoadingScreenDuration);
		TimeLoadingScreenShown = 0.;
	}

	FSlateApplication::Get().OnPostTick().RemoveAll(this);

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

bool UGameLoadingScreenManager::IsShowingInitialLoadingScreen() const
{
	const FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get();
	return PreLoadScreenManager != nullptr && PreLoadScreenManager->HasValidActivePreLoadScreen();
}
