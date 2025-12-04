#pragma once

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameBaseIndicatorWidget.h"
#include "WNameplateWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class UWcTargetingBehaviorComponent;
struct FTargetSelectedMessage;

UCLASS(Abstract)
class WOWUI_API UWNameplateWidget final : public UGameBaseIndicatorWidget
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
	TWeakObjectPtr<AActor> ActorToAttach;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Name_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> LeftTargetArrow_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> RightTargetArrow_Image;
};
