// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Blueprint/UserWidget.h"
#include "GameBaseIndicatorWidget.generated.h"

class UGameIndicatorDescriptor;

/**
 * @brief All markers using Indicator System have to inherit this user widget.
 */
UCLASS(Abstract, ClassGroup=UI)
class GAMEINDICATORSYSTEM_API UGameBaseIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeIndicatorWidget(const TWeakObjectPtr<UGameIndicatorDescriptor> InIndicatorDescriptor);
	void PlayIndicatorRemovedAnimation();

public:
	
	/* This function is called once the indicator is determined to be clamped or not */
	virtual void NativeOnClampedStatusChanged(const bool bIsClamped);
	
	/* This function is called for playing a sound for indicator removal etc. */
	virtual void NativeOnIndicatorPreDestruction() {}

	/* Use this function to reset all properties that may have changed during the indicator's lifetime.
	 * Otherwise, these properties will be kept for the next indicator, which is in most cases undesired. */
    virtual void NativeResetWidgetToDefault() {}

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnIndicatorRemovedAnimationFinished, UGameBaseIndicatorWidget* IndicatorWidget)
	FOnIndicatorRemovedAnimationFinished OnIndicatorRemovedAnimationFinished;

	UFUNCTION()
	void HandleIndicatorRemovedAnimationFinished();

	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	TWeakObjectPtr<UGameIndicatorDescriptor> IndicatorDescriptor;
	
protected:
	
	/* Called when an indicator appears on the screen. Here you should do all initialization (basically NativeConstruct).
	 * Do not use NativeConstruct */
	virtual void NativeOnIndicatorWidgetInitialized() {}

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected: // Set By The Editor

	UPROPERTY(EditAnywhere, Category = "Indicator Behavior")
	bool bCollapseWhenClamped = false;

protected:
	TWeakObjectPtr<UObject> DataObject;
	
private: // Internal

	bool bMarkedForRemoval = false;

private: // Widget Bindings

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> IndicatorAdded_Anim;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> IndicatorRemoved_Anim;
};
