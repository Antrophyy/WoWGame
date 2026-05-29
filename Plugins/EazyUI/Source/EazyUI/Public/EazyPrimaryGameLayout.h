#pragma once

#include "CommonUIExtensions.h"
#include "CommonUserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
#include "LogEazyUI.h"
#include "Foundation/EazyCommonActivatableWidget.h"
#include "EazyPrimaryGameLayout.generated.h"

class UEazyCommonActivatableWidgetStack;
class UEazyCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UCommonActivatableWidgetContainerBase;

/**
 * The state of an async load operation for the UI.
 */
enum class EAsyncWidgetLayerState : uint8
{
	// The load handle was canceled before it could be finished.
	Canceled,
	// After widget is constructed. This is the state before the widget is activated.
	PostConstruct,
	// NativeOnActivated may have already run.
	PostActivation
};

/**
 * The primary game UI layout of your game.  This widget class represents how to lay out, push and display all layers
 * of the UI for a single player.
 */
UCLASS(Abstract, meta = (DisableNativeTick), ClassGroup=UI)
class EAZYUI_API UEazyPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	static UEazyPrimaryGameLayout* GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject);
	static UEazyPrimaryGameLayout* GetPrimaryGameLayout(const APlayerController* PlayerController);
	static UEazyPrimaryGameLayout* GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer);

public:
	explicit UEazyPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	template <typename ActivatableWidgetT = UEazyCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UEazyCommonActivatableWidget> ActivatableWidgetClass)
	{
		return PushWidgetToLayerAsync<ActivatableWidgetT>(LayerName, bSuspendInputUntilComplete, ActivatableWidgetClass, [](EAsyncWidgetLayerState, ActivatableWidgetT*) {});
	}

	template <typename ActivatableWidgetT = UEazyCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerAsync(FGameplayTag LayerName, const bool bSuspendInputUntilComplete, TSoftClassPtr<UEazyCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetLayerState, ActivatableWidgetT*)> StateFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UEazyCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (!ensureAlwaysMsgf(!ActivatableWidgetClass.IsNull(), TEXT("[PushWidgetToLayerAsync] Attempting to push null activatable widget class to stack %s"), *LayerName.ToString()))
		{
			return TSharedPtr<FStreamableHandle>();
		}
		
		static FName NAME_PushingWidgetToLayer("PushingWidgetToLayer");
		const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer) : NAME_None;
	
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
	        [this, LayerName, ActivatableWidgetClass, StateFunc, SuspendInputToken]
	        {
	            UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);

	            ActivatableWidgetT* Widget = PushWidgetToLayer<ActivatableWidgetT>(LayerName, ActivatableWidgetClass.Get(), [StateFunc](ActivatableWidgetT& WidgetToInit)
	            {
		            StateFunc(EAsyncWidgetLayerState::PostConstruct, &WidgetToInit);
	            });
	        	
	            StateFunc(EAsyncWidgetLayerState::PostActivation, Widget);
	        })
		, FStreamableManager::AsyncLoadHighPriority);
		
		// Set up a cancel delegate so that we can resume input if this handler is canceled.
		StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
		  [this, StateFunc, LayerName, SuspendInputToken]
		  {
		      UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);
		      StateFunc(EAsyncWidgetLayerState::Canceled, nullptr);
		  	  UE_LOG(LogEazyUI, Display, TEXT("[PushWidgetToLayerAsync] Async load of widget in stack %s was cancelled"), *LayerName.ToString());
		  })
		);

		return StreamingHandle;
	}

	template <typename ActivatableWidgetT = UEazyCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayer(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayer<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UEazyCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayer(const FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UEazyCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (!IsValid(ActivatableWidgetClass))
		{
			ensureAlwaysMsgf(false, TEXT("[PushWidgetToLayer] Attempting to push null activatable widget class to stack %s."), *LayerName.ToString());
			return nullptr;
		}
		
		UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName);
		if (IsValid(Layer))
		{
			UE_LOG(LogEazyUI, Display, TEXT("[PushWidgetToLayer] Pushing widget with name %s to stack %s"), *ActivatableWidgetClass->GetName(), *LayerName.ToString());
			ActivatableWidgetT* PushedWidget = Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);

			if (IsValid(PushedWidget))
			{
				PushedWidget->NativeOnPushed();
			}
			
			return PushedWidget;
		}

		ensureAlwaysMsgf(IsValid(Layer), TEXT("[PushWidgetToLayer] Attempting to Push widget %s to nonexistent %s layer stack!"), *ActivatableWidgetClass->GetName(), *LayerName.ToString());
		return nullptr;
	}

	// Overloaded function that removes it from a specific layer.
	void FindAndRemoveWidgetFromLayer(UEazyCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName) const;

	// Remove all widgets from the layer.
	void ClearWidgetsFromLayer(const FGameplayTag LayerName) const;

	// Returns true if there are no widgets in the layer.
	bool IsLayerClear(const FGameplayTag LayerName) const;

	// Get the layer widget for the given layer tag.
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName) const;

	// A dormant root layout is collapsed and responds only to persistent actions registered by the owning player.
	void SetIsDormant(bool bInDormant);
	bool IsDormant() const { return bIsDormant; }

	virtual void OnIsDormantChanged();

protected:
	// Register a layer that widgets can be pushed onto.
	UFUNCTION(Category="Layer")
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	void OnLayerTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool IsTransitioning);

private:

	bool bIsDormant = false;

	// Lets us keep track of all suspended input tokens so that multiple async UIs can be loading, and we correctly suspend
	// for the duration of all of them.
	TArray<FName> SuspendInputTokens;

	// The layers registered for the primary layout.
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> Layers;
};
