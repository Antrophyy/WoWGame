#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/GameInstance.h"
#include "UObject/WeakInterfacePtr.h"
#include <Misc/ConfigCacheIni.h>
#include "Engine/EngineTypes.h"
#include "GameLoadingScreenManager.generated.h"

class UGameLoadingScreenData;
class IGameLoadingProcess;
class UTexture2D;
class IInputProcessor;
class SWidget;

UCLASS(Abstract, ClassGroup=UI)
class GAMELOADINGSCREEN_API UGameLoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public: // GameInstanceSubsystem overriden functions.

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Deinitialize() override;

public: // TickableGameObject overriden functions.

	virtual void Tick(const float DeltaTime) override;

	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Conditional; }

	virtual bool IsTickable() const override { return !HasAnyFlags(RF_ClassDefaultObject); }

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGameLoadingScreenManager, STATGROUP_Tickables); }

	virtual UWorld* GetTickableGameObjectWorld() const override { return GetGameInstance()->GetWorld(); }

public: // Manager functions

	static UGameLoadingScreenManager* Get(const UObject* WorldContextObject);

	FString GetReasonForShowingOrHidingLoadingScreen() const { return DebugLoadingScreenReason; }

	UGameLoadingScreenData* GetLoadingScreenData() const { return LoadingScreenData; }

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
	void RegisterLoadingProcessor(TScriptInterface<IGameLoadingProcess> Interface);

	/**
	 * @brief Unregister passed in class. The manager will stop listening for whether to show a loading screen.
	 * @param Interface reference to class that implements IGameLoadingProcess interface.
	 */
	void UnregisterLoadingProcessor(TScriptInterface<IGameLoadingProcess> Interface);

	UTexture2D* GetRandomBackgroundImage();

	FText GetRandomTipOrHint() const;

protected:

	// If we're not blocking a main thread (e.g. in a load map), we can show the loading screen on the main thread.
	virtual void StartLoadingScreenOnMainThread() const;
	virtual void StopLoadingScreenOnMainThread();
	
	void StartLoadingScreenOnSlateThread() const;

	virtual void OnLoadingScreenStarted() {}
	virtual void OnLoadingScreenStopped() {}

	// Overridable function that should return a new loading screen slate widget.
	virtual TSharedPtr<SWidget> GetLoadingScreenWidget();

	// True when we are between PreLoadMap and PostLoadMap.
	bool bCurrentlyInLoadMap = false;

	bool bWaitForTextureStreaming = false;

	// The time when we started showing the loading screen.
	double TimeLoadingScreenShown = 0.0;

	TSharedPtr<SWidget> LoadingScreenWidget;
	
private:

	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);

	void HandlePostLoadMap(UWorld* World);

	// Determines if we should show or hide the loading screen. Called every frame.
	void UpdateLoadingScreen();

	// Returns true if we need to be showing the loading screen.
	bool CheckForAnyNeedToShowLoadingScreen();

	// Returns true if we want to be showing the loading screen (if we need to or are artificially forcing it on for other reasons).
	bool ShouldShowLoadingScreen();

	// Shows the loading screen. Sets up the loading screen widget on the viewport.
	void ShowLoadingScreen();

	// Hides the loading screen. The loading screen widget will be destroyed.
	void HideLoadingScreen();

	void ChangePerformanceSettings(const bool bEnablingLoadingScreen);

	void SetDebugLoadingScreenReason(const FString& Reason);

	void AsyncLoadAllBackgroundImages();

	void StartBlockingInput();

	void StopBlockingInput();

private:

	UPROPERTY()
	TArray<TObjectPtr<UTexture2D>> LoadedBackgroundImages;

	FTimerHandle TimerBeforeReEnablingGlobalInvalidation;

	/** Input processor to eat all input while the loading screen is shown */
	TSharedPtr<IInputProcessor> InputPreProcessor;
	
	// Delegate broadcast when the reason for loading screen changes.
	FOnLoadingScreenReasonChangedDelegate LoadingScreenReasonChanged;

	UPROPERTY()
	TObjectPtr<UGameLoadingScreenData> LoadingScreenData;

	// External loading processors, components maybe actors that delay the loading.
	TArray<TWeakInterfacePtr<IGameLoadingProcess>> ExternalLoadingProcessors;

	// The reason why the loading screen is up (or not).
	FString DebugLoadingScreenReason;

	FString MapToBeLoaded;

	// The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requirement).
	double TimeLoadingScreenLastDismissed = -1.0;

	// The time until the next log for why the loading screen is still up.
	double TimeUntilNextLogHeartbeatSeconds = 0.0;

	// True when the loading screen is currently being shown.
	bool bCurrentlyShowingLoadingScreen = false;

	bool bGlobalInvalidationEnabled = false;
	
	bool bFastLoadingEnabled = false;
};
