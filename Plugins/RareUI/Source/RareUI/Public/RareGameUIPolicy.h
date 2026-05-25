#pragma once

#include "Core/RareUIActionData.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "RareGameUIPolicy.generated.h"

class URareInputAction;
class URareActionKeyWidget;
class URareGameDialog;
class UInputMappingContext;
class ULocalPlayer;
class URarePrimaryGameLayout;
class URareGameUIManagerSubsystem;

UENUM()
enum class ELocalMultiplayerInteractionMode : uint8
{
	// Fullscreen viewport for the primary player only, regardless of the other player's existence
	PrimaryOnly,

	// Fullscreen viewport for one player, but players can swap control over who's is displayed and who's is dormant
	SingleToggle,

	// Viewports displayed simultaneously for both players
	Simultaneous
};

USTRUCT(Category=UI)
struct RAREUI_API FRootViewportLayoutInfo
{
	GENERATED_BODY()

	FRootViewportLayoutInfo() {}

	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, URarePrimaryGameLayout* InRootLayout, const bool IsInViewport)
		: LocalPlayer(InLocalPlayer)
		  , RootLayout(InRootLayout)
		  , bAddedToViewport(IsInViewport) {}

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }

	UPROPERTY(Transient)
	ULocalPlayer* LocalPlayer = nullptr;

	UPROPERTY(Transient)
	URarePrimaryGameLayout* RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;
};

UCLASS(Abstract, Blueprintable, Within = RareGameUIManagerSubsystem, ClassGroup = UI)
class RAREUI_API URareGameUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	template <typename RareUIPolicyClass = URareGameUIPolicy>
	static RareUIPolicyClass* GetGameUIPolicyAs(const UObject* WorldContextObject)
	{
		return Cast<RareUIPolicyClass>(GetGameUIPolicy(WorldContextObject));
	}

	static URareGameUIPolicy* GetGameUIPolicy(const UObject* WorldContextObject);

public:
	virtual UWorld* GetWorld() const override;
	URareGameUIManagerSubsystem* GetOwningUIManager() const;
	URarePrimaryGameLayout* GetRootLayout(const ULocalPlayer* LocalPlayer) const;
	void RequestPrimaryControl(URarePrimaryGameLayout* Layout);

public:
	void AddLayoutToViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout);
	void RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout);

	virtual void OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout);
	virtual void OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout);
	virtual void OnRootLayoutReleased(ULocalPlayer* LocalPlayer, URarePrimaryGameLayout* Layout);

	void CreateLayoutWidget(ULocalPlayer* LocalPlayer);
	TSubclassOf<URarePrimaryGameLayout> GetLayoutWidgetClass(ULocalPlayer* LocalPlayer);
	TSoftClassPtr<URareGameDialog> GetBaseDialogClass() const;

	void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer);
	
	TSubclassOf<URareActionKeyWidget> GetActionKeyWidget();
	const FRareUIActionData& GetBackInputActionData();

protected: // Set by the Editor

	UPROPERTY(EditAnywhere, Category = "Layout")
	TSoftClassPtr<URarePrimaryGameLayout> LayoutClass;

	// The input mapping context to use for all UI widgets. These IMCs are added all the time and filtered using InputMode tags on EI.
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> UserInterfaceInputMappingContexts;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TSubclassOf<URareActionKeyWidget> DefaultEnhancedActionWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FRareUIActionData BackActionData;

	UPROPERTY(EditAnywhere, Category = "Dialogs")
	TSoftClassPtr<URareGameDialog> BaseDialog_Class;

private: // Internal

	// For a single local player experience this will only contain one layout.
	// But if needs be this can be scaled up so that it contains multiple local players.
	UPROPERTY(Transient)
	TArray<FRootViewportLayoutInfo> RootViewportLayouts;

	ELocalMultiplayerInteractionMode LocalMultiplayerInteractionMode = ELocalMultiplayerInteractionMode::PrimaryOnly;
};
