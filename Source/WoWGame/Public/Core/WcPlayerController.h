#pragma once

#include "GameFramework/PlayerController.h"
#include "WcPlayerController.generated.h"

UCLASS()
class WOWGAME_API AWcPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void ReceivedPlayer() override;
};
