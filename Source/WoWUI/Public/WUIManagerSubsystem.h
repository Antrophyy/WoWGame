#pragma once

#include "RareGameUIManagerSubsystem.h"
#include "WUIManagerSubsystem.generated.h"

UCLASS()
class WOWUI_API UWUIManagerSubsystem final : public URareGameUIManagerSubsystem
{
	GENERATED_BODY()

protected:
	virtual void SyncHudVisibilityForPlayer(const bool bIsShown, const APlayerController* OwningPlayerController) override;

private:
	TMap<TWeakObjectPtr<ULocalPlayer>, FDelegateHandle> PlayerControllerSetDelegatesForLocalPlayer;
};
