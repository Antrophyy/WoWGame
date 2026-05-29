#pragma once

#include "EazyUIActionData.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "EazyGameUIPolicy.generated.h"

class UEazyInputAction;
class UEazyActionKeyWidget;
class UEazyGameDialog;
class UInputMappingContext;
class ULocalPlayer;
class UEazyPrimaryGameLayout;
class UEazyGameUIManagerSubsystem;

UENUM()
enum class ELocalMultiplayerInteractionMode : uint8
{
	// Fullscreen viewport for the primary player only, regardless of the other player's existence
	PrimaryOnly,

	// Fullscreen viewport for one player, but players can swap control over whose is displayed and whose is dormant
	SingleToggle,

	// Viewports displayed simultaneously for both players
	Simultaneous
};

USTRUCT(Category=UI)
struct EAZYUI_API FRootViewportLayoutInfo
{
	GENERATED_BODY()

	FRootViewportLayoutInfo() {}

	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UEazyPrimaryGameLayout* InRootLayout, const bool IsInViewport)
		: LocalPlayer(InLocalPlayer)
		  , RootLayout(InRootLayout)
		  , bAddedToViewport(IsInViewport) {}

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }

	UPROPERTY(Transient)
	ULocalPlayer* LocalPlayer = nullptr;

	UPROPERTY(Transient)
	UEazyPrimaryGameLayout* RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;
};

UCLASS(Abstract, Blueprintable, Within = EazyGameUIManagerSubsystem, ClassGroup = UI)
class EAZYUI_API UEazyGameUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	template <typename EazyUIPolicyClass = UEazyGameUIPolicy>
	static EazyUIPolicyClass* GetGameUIPolicyAs(const UObject* WorldContextObject)
	{
		return Cast<EazyUIPolicyClass>(GetGameUIPolicy(WorldContextObject));
	}

	static UEazyGameUIPolicy* GetGameUIPolicy(const UObject* WorldContextObject);

public:
	virtual UWorld* GetWorld() const override;
	UEazyGameUIManagerSubsystem* GetOwningUIManager() const;
	UEazyPrimaryGameLayout* GetRootLayout(const ULocalPlayer* LocalPlayer) const;
	void RequestPrimaryControl(UEazyPrimaryGameLayout* Layout);

public:
	void AddLayoutToViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout);
	void RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout);

	virtual void OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout);
	virtual void OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout);
	virtual void OnRootLayoutReleased(ULocalPlayer* LocalPlayer, UEazyPrimaryGameLayout* Layout);

	void CreateLayoutWidget(ULocalPlayer* LocalPlayer);
	TSubclassOf<UEazyPrimaryGameLayout> GetLayoutWidgetClass(ULocalPlayer* LocalPlayer) const;
	TSoftClassPtr<UEazyGameDialog> GetBaseDialogClass() const;

	void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer);
	
	TSubclassOf<UEazyActionKeyWidget> GetActionKeyWidget();
	const FEazyUIActionData& GetBackInputActionData();

protected: // Set by the Editor

	UPROPERTY(EditAnywhere, Category = "Layout")
	TSoftClassPtr<UEazyPrimaryGameLayout> LayoutClass;

	// The input mapping context to use for all UI widgets. These IMCs are added all the time and filtered using InputMode tags on EI.
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> UserInterfaceInputMappingContexts;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TSubclassOf<UEazyActionKeyWidget> DefaultEnhancedActionWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FEazyUIActionData BackActionData;

	UPROPERTY(EditAnywhere, Category = "Dialogs")
	TSoftClassPtr<UEazyGameDialog> BaseDialog_Class;

private: // Internal

	// For a single local player experience this will only contain one layout.
	// But if needs be this can be scaled up so that it contains multiple local players.
	UPROPERTY(Transient)
	TArray<FRootViewportLayoutInfo> RootViewportLayouts;

	ELocalMultiplayerInteractionMode LocalMultiplayerInteractionMode = ELocalMultiplayerInteractionMode::PrimaryOnly;
};
