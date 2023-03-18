#pragma once

#include "GameUIManagerSubsystem.h"
#include "WcUIManagerSubsystem.generated.h"

UCLASS()
class WOWUI_API UWcUIManagerSubsystem final : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

protected:

	virtual void Deinitialize() override;

	virtual void NotifyPlayerAdded(ULocalPlayer* LocalPlayer) override;

	virtual void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer) override;

	virtual void HandleHUDVisibilityChanged(const bool bIsShown, const APlayerController* OwningPlayerController) override;

private:

	TMap<TWeakObjectPtr<ULocalPlayer>, FDelegateHandle> PlayerControllerSetDelegatesForLocalPlayer;
};
