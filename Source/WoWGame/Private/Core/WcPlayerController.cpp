#include "Core/WcPlayerController.h"
#include "Core/WcLocalPlayer.h"

void AWcPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (UWcLocalPlayer* WcLocalPlayer = Cast<UWcLocalPlayer>(GetLocalPlayer()))
	{
		WcLocalPlayer->OnPlayerControllerSet().Broadcast(this);
	}
}
