#pragma once

#include "GameFramework/GameplayMessageSubsystem.h"
#include "IndicatorSystem/BaseIndicatorWidget.h"
#include "WcNameplateWidget.generated.h"

class UImage;
class UProgressBar;
class UWcTextBlock;
class UWcTargetingBehaviorComponent;
struct FTargetSelectedMessage;

UCLASS(Abstract)
class WOWUI_API UWcNameplateWidget final : public UBaseIndicatorWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnIndicatorWidgetInitialized() override;

	virtual void NativeResetWidgetToDefault() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void MakePrimaryTarget(const bool bPrimaryTarget) const;

private: // Broadcast Handling

	void HandleTargetSelectionChanged(const FGameplayTag GameplayTag, const FTargetSelectedMessage& Message);

	FGameplayMessageListenerHandle TargetSelectedListener;

	TWeakObjectPtr<UWcTargetingBehaviorComponent> TargetingBehaviorComponent;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcTextBlock> Name_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> LeftTargetArrow_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> RightTargetArrow_Image;
};
