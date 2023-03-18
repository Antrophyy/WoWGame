#pragma once

#include "Engine/LocalPlayer.h"
#include "WcLocalPlayer.generated.h"

DECLARE_EVENT_OneParam(UWcLocalPlayer, FPlayerControllerSetEvent, APlayerController*);

UCLASS()
class WOWGAME_API UWcLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	FPlayerControllerSetEvent& OnPlayerControllerSet() { return PlayerControllerSet; }

private:

	FPlayerControllerSetEvent PlayerControllerSet;
};
