#pragma once

#include "Engine/GameInstance.h"
#include "GameUIManagerSubsystem.generated.h"

class AExtensionHUD;
class UPrimaryGameLayout;

USTRUCT()
struct UIEXTENSIONS_API FRootViewportLayoutInfo
{
	GENERATED_BODY()

	FRootViewportLayoutInfo() {}

	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UPrimaryGameLayout* InRootLayout, const bool IsInViewport)
		: LocalPlayer(InLocalPlayer)
		  , RootLayout(InRootLayout)
		  , bAddedToViewport(IsInViewport) {}

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }
	
	UPROPERTY(Transient)
	ULocalPlayer* LocalPlayer = nullptr;

	UPROPERTY(Transient)
	UPrimaryGameLayout* RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;
	
};

/**
 * This manager is intended to be replaced by whatever your game needs to
 * actually create, so this class is abstract to prevent it from being created.
 * 
 * If you just need the basic functionality you will start by subclassing this
 * subsystem in your own game.
 */
UCLASS(Abstract, config = Game, ClassGroup=UI)
class UIEXTENSIONS_API UGameUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);

	virtual void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);

	virtual void NotifyPlayerHUDInitialized(AExtensionHUD* HUD);

	UPrimaryGameLayout* GetRootLayout(const ULocalPlayer* LocalPlayer) const;

protected:
	void AddLayoutToViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout);

	void RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout);

	virtual void OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout);

	virtual void OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout) {}

	void CreateLayoutWidget(ULocalPlayer* LocalPlayer);

	TSubclassOf<UPrimaryGameLayout> GetLayoutWidgetClass(ULocalPlayer* LocalPlayer) const;

	void AddLayoutToViewportForPlayer(APlayerController* PlayerController);

	/**
	 * @brief Method gets called whenever ShowHUD from AHUD class gets called. Can be overriden and used to hide specific layer
	 * In other words, make Game Layer act the same way like HUD for example.
	 * @param bIsShown Whether the HUD is currently shown or hidden.
	 * @param OwningPlayerController PlayerController that owns the HUD.
	 */
	virtual void HandleHUDVisibilityChanged(const bool bIsShown, const APlayerController* OwningPlayerController) {}

protected:

	TSoftObjectPtr<AExtensionHUD> PlayerHUD;

private:
	
	// For a single local player experience this will only contain one layout.
	// But if needs be this can be scaled up so that it contains multiple local players.
	UPROPERTY(Transient)
	TArray<FRootViewportLayoutInfo> RootViewportLayouts;

	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<UPrimaryGameLayout> LayoutClass;

	const int32 ViewportLayoutDefaultZOrder = 1000;

	FDelegateHandle LocalPlayerRemovedHandle;

	FDelegateHandle LocalPlayerAddedHandle;

	FDelegateHandle HUDVisibilityChangedHandle;
	
};
