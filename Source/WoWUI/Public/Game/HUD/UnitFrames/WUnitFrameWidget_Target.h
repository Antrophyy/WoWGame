#pragma once

#include "WUnitFrameWidget_Base.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "WUnitFrameWidget_Target.generated.h"

struct FTargetSelectedMessage;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWUnitFrameWidget_Target final : public UWUnitFrameWidget_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private: // Internal

	void HandleTargetSelectionChanged(const FGameplayTag GameplayTag, const FTargetSelectedMessage& Message);
	FGameplayMessageListenerHandle TargetSelectedListener;
};
