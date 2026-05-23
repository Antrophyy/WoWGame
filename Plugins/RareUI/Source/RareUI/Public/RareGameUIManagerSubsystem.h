// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Engine/GameInstance.h"
#include "Containers/Ticker.h"
#include "Engine/DeveloperSettings.h"
#include "RareGameUIManagerSubsystem.generated.h"

class UWidget;
class URareGameUIPolicy;
class ARareHUD;
class URarePrimaryGameLayout;

/**
 * This manager is intended to be replaced by whatever your game needs to
 * actually create, so this class is abstract to prevent it from being created.
 * 
 * If you just need the basic functionality you will start by subclassing this
 * subsystem in your own game.
 */
UCLASS(Abstract, config = Engine, ClassGroup = UI)
class RAREUI_API URareGameUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	URareGameUIPolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }

	virtual void NotifyLocalPlayerAdded(ULocalPlayer* InLocalPlayer);
	virtual void NotifyLocalPlayerRemoved(ULocalPlayer* InLocalPlayer);
	
	virtual void NotifyPlayerAdded(APlayerController* PlayerController);
	virtual void NotifyPlayerRemoved(APlayerController* PlayerController);
	virtual void NotifyPlayerHUDInitialized(ARareHUD* HUD);

protected:
	
	/**
	 * @brief Can be overriden and used to hide specific layer
	 * In other words, make Game Layer act the same way as HUD for example.
	 * @param bIsShown Whether the HUD is currently shown or hidden.
	 * @param OwningPlayerController PlayerController that owns the HUD.
	 */
	virtual void SyncHudVisibilityForPlayer(const bool bIsShown, const APlayerController* OwningPlayerController);

	void SwitchToPolicy(URareGameUIPolicy* InPolicy);

	bool Tick(const float DeltaTime);

	void SyncRootLayoutVisibilityToShowHUD();

	virtual void OnWidgetAddedToViewport(UWidget* InAddedWidget, ULocalPlayer* InLocalPlayer) const;

protected:
	TWeakObjectPtr<ARareHUD> PlayerHUD;

private:
	UPROPERTY(Transient)
	TObjectPtr<URareGameUIPolicy> CurrentPolicy;
	
	UPROPERTY(config, EditAnywhere, Category = "Policy")
	TSoftClassPtr<URareGameUIPolicy> DefaultUIPolicyClass;
	
	FDelegateHandle LocalPlayerRemovedHandle;
	FDelegateHandle LocalPlayerAddedHandle;
	
	FTSTicker::FDelegateHandle TickHandle;
};
