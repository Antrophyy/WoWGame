#include "Core/EazyViewModelBase.h"
#include "GameFramework/PlayerState.h"

UWorld* UEazyViewModelBase::GetWorld() const
{
	return OwningLocalPlayer.IsValid() ? OwningLocalPlayer->GetWorld() : nullptr;
}

void UEazyViewModelBase::InitializeViewModel(ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
	{
		return;
	}

	OwningLocalPlayer = InLocalPlayer;
	OnCreated();
}

void UEazyViewModelBase::DeinitializeViewModel()
{
	OnDestroyed();
	OwningLocalPlayer.Reset();
}

void UEazyViewModelBase::OnCreated()
{
	BP_OnCreated();
	OnCreatedEvent.Broadcast();
}

void UEazyViewModelBase::OnDestroyed()
{
	BP_OnDestroyed();
	OnDestroyedEvent.Broadcast();
}

ULocalPlayer* UEazyViewModelBase::BP_GetOwningLocalPlayer() const
{
	return GetOwningLocalPlayer();
}

APlayerController* UEazyViewModelBase::BP_GetOwningPlayer() const
{
	return GetOwningPlayer();
}

APawn* UEazyViewModelBase::BP_GetOwningPlayerPawn() const
{
	return GetOwningPlayerPawn();
}

APlayerState* UEazyViewModelBase::BP_GetOwningPlayerState() const
{
	return GetOwningPlayerState<APlayerState>();
}
