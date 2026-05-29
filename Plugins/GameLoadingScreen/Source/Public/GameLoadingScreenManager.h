#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include <Misc/ConfigCacheIni.h>

#include "MoviePlayer.h"
#include "Engine/EngineTypes.h"
#include "GameLoadingScreenManager.generated.h"

struct FGameplayTag;
class UGameLoadingScreenData;
class IGameLoadingProcess;
class UTexture2D;
class IInputProcessor;
class SWidget;
class FViewport;

UCLASS(ClassGroup=UI)
class GAMELOADINGSCREEN_API UGameLoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public: // GameInstanceSubsystem overriden functions.

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Deinitialize() override;
	
	static UGameLoadingScreenManager* Get(const UObject* WorldContextObject);

public: // TickableGameObject overriden functions.

	virtual void Tick(const float DeltaTime) override;
	void SlateTick(const float DeltaTime);

	virtual ETickableTickType GetTickableTickType() const override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGameLoadingScreenManager, STATGROUP_Tickables); }

	virtual UWorld* GetTickableGameObjectWorld() const override { return GetGameInstance()->GetWorld(); }
	
	virtual bool IsTickableWhenPaused() const override { return true; }

public: // Manager functions

	FString GetReasonForShowingOrHidingLoadingScreen() const { return DebugLoadingScreenReason; }

	bool IsShowingLoadingScreen() const { return bCurrentlyShowingLoadingScreen; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool, bIsVisible);
	// Delegate broadcast when the loading screen visibility changes.
	UPROPERTY(BlueprintAssignable, Category = "LoadingScreen")
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

	// Called when the loading screen visibility changes.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenReasonChangedDelegate, const FString&);
	FOnLoadingScreenReasonChangedDelegate& OnLoadingScreenReasonChangedDelegate() { return LoadingScreenReasonChanged; }

	/**
	 * @brief Register passed in class that implements IGameLoadingProcess interface and ShouldShowLoadingScreen function.
	 * If a class is registered it will be checked whether it needs to show loading screen. Don't forget to unregister as well.
	 * @param Interface reference to class that implements IGameLoadingProcess interface.
	 */
	UFUNCTION(BlueprintCallable, Category="UI|LoadingScreen")
	void RegisterLoadingProcessor(TScriptInterface<IGameLoadingProcess> Interface);

	/**
	 * @brief Unregister passed in class. The manager will stop listening for whether to show a loading screen.
	 * @param Interface reference to class that implements IGameLoadingProcess interface.
	 */
	UFUNCTION(BlueprintCallable, Category="UI|LoadingScreen")
	void UnregisterLoadingProcessor(TScriptInterface<IGameLoadingProcess> Interface);
	
	/** Delegate providing functionality for beginning streaming pause. */
	FBeginStreamingPauseDelegate BeginDelegate;

	/** Delegate providing functionality for ending streaming pause. */
	FEndStreamingPauseDelegate EndDelegate;
	
	/** Override functionality for beginning/ending streaming pause. */
	virtual void BeginStreamingPause(FViewport* Viewport );
	virtual void StopStreamingPause();
protected:

	void StartLoadingScreen();
	
	virtual void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	virtual void HandlePostLoadMap(UWorld* World);
	virtual void OnLoadingScreenStarted() {}
	virtual void OnLoadingScreenStopped() {}

	// The time when we started showing the loading screen.
	double TimeLoadingScreenShown = 0.0;

	TSharedPtr<SWidget> LoadingScreenWidget;
	
	// True when the loading screen is currently being shown.
	bool bCurrentlyShowingLoadingScreen = false;
	
	// Returns true if we want to be showing the loading screen (if we need to or are artificially forcing it on for other reasons).
	virtual bool ShouldShowLoadingScreen();
	
private:
	// Determines if we should show or hide the loading screen. Called every frame.
	void UpdateLoadingScreen();

	// Returns true if we need to be showing the loading screen.
	bool CheckForAnyNeedToShowLoadingScreen();

	// Shows the loading screen. Sets up the loading screen widget on the viewport.
	void ShowLoadingScreen();

	// Hides the loading screen. The loading screen widget will be destroyed.
	void HideLoadingScreen();

	void ChangePerformanceSettings(const bool bEnablingLoadingScreen);

	void SetDebugLoadingScreenReason(const FString& Reason);

	void StartBlockingInput();

	void StopBlockingInput();
	
	bool IsShowingInitialLoadingScreen() const;

private:
	FTimerHandle TimerBeforeReEnablingGlobalInvalidation;

	/** Input processor to eat all input while the loading screen is shown */
	TSharedPtr<IInputProcessor> InputPreProcessor;
	
	// Delegate broadcast when the reason for loading screen changes.
	FOnLoadingScreenReasonChangedDelegate LoadingScreenReasonChanged;

	// External loading processors, components maybe actors that delay the loading.
	TArray<TWeakObjectPtr<UObject>> ExternalLoadingProcessors;

	// The reason why the loading screen is up (or not).
	FString DebugLoadingScreenReason;
	
	// The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requirement).
	double TimeLoadingScreenLastDismissed = -1.0;

	// The time until the next log for why the loading screen is still up.
	double TimeUntilNextLogHeartbeatSeconds = 0.0;

	bool bGlobalInvalidationEnabled = false;
	
	bool bFastLoadingEnabled = false;
	
	bool bCurrentlyInLoadMap = false;
};
