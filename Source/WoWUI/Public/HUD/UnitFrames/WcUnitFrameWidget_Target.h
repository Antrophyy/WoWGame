#pragma once

#include "WcUnitFrameWidget_Base.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "WcUnitFrameWidget_Target.generated.h"

struct FTargetSelectedMessage;

UCLASS(Abstract, Blueprintable)
class WOWUI_API UWcUnitFrameWidget_Target final : public UWcUnitFrameWidget_Base
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

private: // Broadcast Handling

	void HandleTargetSelectionChanged(const FGameplayTag GameplayTag, const FTargetSelectedMessage& Message);

	FGameplayMessageListenerHandle TargetSelectedListener;
};
