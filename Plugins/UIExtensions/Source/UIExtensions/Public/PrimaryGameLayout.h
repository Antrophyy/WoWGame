#pragma once

#include "CommonUIExtensions.h"
#include "CommonUserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
#include "PrimaryGameLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UCommonActivatableWidgetContainerBase;

/**
 * The state of an async load operation for the UI.
 */
enum class EAsyncWidgetLayerState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};

/**
 * The primary game UI layout of your game.  This widget class represents how to layout, push and display all layers
 * of the UI for a single player.
 */
UCLASS(Abstract, meta = (DisableNativeTick), ClassGroup=UI)
class UIEXTENSIONS_API UPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	static UPrimaryGameLayout* GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject);
	static UPrimaryGameLayout* GetPrimaryGameLayout(const APlayerController* PlayerController);
	static UPrimaryGameLayout* GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer);

public:
	UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass)
	{
		return PushWidgetToLayerAsync<ActivatableWidgetT>(LayerName, bSuspendInputUntilComplete, ActivatableWidgetClass, [](EAsyncWidgetLayerState, ActivatableWidgetT*) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerAsync(FGameplayTag LayerName, const bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetLayerState, ActivatableWidgetT*)> StateFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		static FName NAME_PushingWidgetToLayer("PushingWidgetToLayer");
		const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer) : NAME_None;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
	        [this, LayerName, ActivatableWidgetClass, StateFunc, SuspendInputToken]()
	        {
	            UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);

	            ActivatableWidgetT* Widget = PushWidgetToLayer<ActivatableWidgetT>(LayerName, ActivatableWidgetClass.Get(), [StateFunc](ActivatableWidgetT& WidgetToInit)
	            {
	                StateFunc(EAsyncWidgetLayerState::Initialize, &WidgetToInit);
	            });

	            StateFunc(EAsyncWidgetLayerState::AfterPush, Widget);
	        })
		);

		// Setup a cancel delegate so that we can resume input if this handler is canceled.
		StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
		  [this, StateFunc, SuspendInputToken]()
		  {
		      UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);
		      StateFunc(EAsyncWidgetLayerState::Canceled, nullptr);
		  })
		);

		return StreamingHandle;
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayer(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayer<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayer(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}
	
	// Overloaded function that removes it from a specific layer.
	void FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName) const;

	// Remove all widgets from the layer.
	void ClearWidgetsFromLayer(const FGameplayTag LayerName) const;

	// Returns true if there are no widgets in the layer.
	bool IsLayerClear(const FGameplayTag LayerName) const;

	// Get the layer widget for the given layer tag.
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName) const;

protected:
	/** Register a layer that widgets can be pushed onto. */
	UFUNCTION(Category="Layer")
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	void OnLayerTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool IsTransitioning);

private:

	// Lets us keep track of all suspended input tokens so that multiple async UIs can be loading and we correctly suspend
	// for the duration of all of them.
	TArray<FName> SuspendInputTokens;

	// The registered layers for the primary layout.
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> Layers;
};
