#pragma once

#include "Foundation/ExtensionUserWidget.h"
#include "BaseIndicatorWidget.generated.h"

class UIndicatorDescriptor;

/**
 * @brief All markers using Indicator System have to inherit this user widget.
 */

UCLASS(Abstract, HideFunctions=(PreConstruct, Construct, Destruct, OnInitialized))
class UIEXTENSIONS_API UBaseIndicatorWidget : public UExtensionUserWidget
{
	GENERATED_BODY()

public:

	void InitializeIndicatorWidget(const TWeakObjectPtr<UIndicatorDescriptor> InIndicatorDescriptor);

	void PlayIndicatorRemovedAnimation(const TFunction<void()> OnRemovedAnimationFinished);

public: // Overridable Functions

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Clamped Status Changed", Category="Indicators")
	void BP_OnClampedStatusChanged(const bool bIsClamped);

	virtual void NativeOnClampedStatusChanged(const bool bIsClamped);

	/**
	 * @brief This function serves for last time things that you can do. For example playing a sound for indicator removal etc.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Indicator Pre Destruction", Category="Indicators")
	void BP_OnIndicatorPreDestruction();

	virtual void NativeOnIndicatorPreDestruction();

	/**
	 * @brief Use this function to reset all properties that you may have changed during the indicator's lifetime.
	 * Otherwise these properties will be kept for the next indicator, which is in most cases undesired.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Reset Widget To Default", Category="Indicators")
	void BP_ResetWidgetToDefault();

	virtual void NativeResetWidgetToDefault();

protected:

	/**
	 * @brief Called when an indicator appears on the screen. Here you should do all initialization (basically NativeConstruct).
	 * Do not use NativeConstruct
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Indicator Widget Initialized", Category="Indicators")
	void BP_OnIndicatorWidgetInitialized();

	virtual void NativeOnIndicatorWidgetInitialized();

	virtual void NativeOnInitialized() override;

protected:

	TWeakObjectPtr<UIndicatorDescriptor> IndicatorDescriptor;

	UPROPERTY(BlueprintReadOnly, Category="Indicators")
	TWeakObjectPtr<AActor> ActorToAttach;

protected: // Widget Bindings

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> IndicatorAddedAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> IndicatorRemovedAnimation;

private:

	bool bMarkedForRemoval = false;

	UPROPERTY()
	TObjectPtr<UUMGSequencePlayer> RemoveAnimationSequencePlayer;
};
