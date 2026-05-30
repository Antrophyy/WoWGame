#include "Examples/WExampleStatusViewModel.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

UWExampleStatusViewModel::UWExampleStatusViewModel()
{
	bIsPersistent = false;
	bAllowMultipleInstances = false;
}

void UWExampleStatusViewModel::OnCreated()
{
	Super::OnCreated();
	
	const APlayerController* OwningPlayerController = GetOwningPlayer();
	const APawn* OwningPawn = GetOwningPlayerPawn();
	const APlayerState* OwningPlayerState = GetOwningPlayerState();

	SetPlayerControllerNameText(FText::FromString(IsValid(OwningPlayerController) ? OwningPlayerController->GetName() : TEXT("No PlayerController")));
	SetPlayerPawnNameText(FText::FromString(IsValid(OwningPawn) ? OwningPawn->GetName() : TEXT("No Pawn")));
	SetPlayerStateNameText(FText::FromString(IsValid(OwningPlayerState) ? OwningPlayerState->GetName() : TEXT("No PlayerState")));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DelayedUpdateTimerHandle,
			this,
			&UWExampleStatusViewModel::HandleDelayedUpdate,
			2.0f,
			false
		);
	}
}

void UWExampleStatusViewModel::OnDestroyed()
{
	Super::OnDestroyed();
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DelayedUpdateTimerHandle);
	}
}

void UWExampleStatusViewModel::HandleDelayedUpdate()
{
	SetPlayerControllerNameText(FText::FromString(TEXT("Updated PlayerController")));
	SetPlayerPawnNameText(FText::FromString(TEXT("Updated Pawn")));
	SetPlayerStateNameText(FText::FromString(TEXT("Updated PlayerState")));
}


