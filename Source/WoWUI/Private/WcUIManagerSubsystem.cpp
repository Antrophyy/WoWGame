#include "WcUIManagerSubsystem.h"
#include "PrimaryGameLayout.h"
#include "UINativeTags.h"
#include "Core/WcLocalPlayer.h"
#include "Core/WcPlayerController.h"

void UWcUIManagerSubsystem::Deinitialize()
{
	for (TTuple<TWeakObjectPtr<ULocalPlayer>, FDelegateHandle> PlayerDelegateTuple : PlayerControllerSetDelegatesForLocalPlayer)
	{
		if (!PlayerDelegateTuple.Key.IsValid())
			continue;

		if (UWcLocalPlayer* WcLocalPlayer = Cast<UWcLocalPlayer>(PlayerDelegateTuple.Key))
		{
			if (PlayerDelegateTuple.Value.IsValid())
			{
				WcLocalPlayer->OnPlayerControllerSet().Remove(PlayerDelegateTuple.Value);
			}
		}
	}

	Super::Deinitialize();
}

void UWcUIManagerSubsystem::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
	Super::NotifyPlayerAdded(LocalPlayer);

	if (UWcLocalPlayer* WcLocalPlayer = Cast<UWcLocalPlayer>(LocalPlayer))
	{
		const FDelegateHandle PlayerControllerSetHandle = WcLocalPlayer->OnPlayerControllerSet().AddUObject(this, &ThisClass::AddLayoutToViewportForPlayer);
		PlayerControllerSetDelegatesForLocalPlayer.Add(LocalPlayer, PlayerControllerSetHandle);
	}
}

void UWcUIManagerSubsystem::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	Super::NotifyPlayerRemoved(LocalPlayer);

	if (const FDelegateHandle* PlayerControllerSetHandle = PlayerControllerSetDelegatesForLocalPlayer.Find(LocalPlayer))
	{
		if (UWcLocalPlayer* WcLocalPlayer = Cast<UWcLocalPlayer>(LocalPlayer->PlayerController))
		{
			WcLocalPlayer->OnPlayerControllerSet().Remove(*PlayerControllerSetHandle);
		}
	}
}

void UWcUIManagerSubsystem::HandleHUDVisibilityChanged(const bool bIsShown, const APlayerController* OwningPlayerController)
{
	if (OwningPlayerController)
	{
		if (const UPrimaryGameLayout* PrimaryGameLayout = GetRootLayout(OwningPlayerController->GetLocalPlayer()))
		{
			if (UCommonActivatableWidgetStack* GameStack = Cast<UCommonActivatableWidgetStack>(PrimaryGameLayout->GetLayerWidget(TAG_UI_LAYER_GAME)))
			{
				GameStack->SetVisibility(bIsShown ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			}
		}
	}
}
