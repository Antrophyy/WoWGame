#include "WUIManagerSubsystem.h"

#include "LogWoWUI.h"
#include "RareGameUIPolicy.h"
#include "RarePrimaryGameLayout.h"
#include "WoWUITags.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UWUIManagerSubsystem::SyncHudVisibilityForPlayer(const bool bIsShown, const APlayerController* OwningPlayerController)
{
	if (!IsValid(GetCurrentUIPolicy()) || !IsValid(OwningPlayerController))
	{
		return;
	}

	if (const URarePrimaryGameLayout* RootLayout = GetCurrentUIPolicy()->GetRootLayout(OwningPlayerController->GetLocalPlayer()))
	{
		const float DesiredOpacity = bIsShown ? 1.f : 0.f;

		if (UCommonActivatableWidgetContainerBase* GameLayer = RootLayout->GetLayerWidget(WoWUITags::Layer::Game))
		{
			if (FMath::IsNearlyEqual(GameLayer->GetRenderOpacity(), DesiredOpacity))
			{
				// We already changed the opacity, no need to do it again.
				return;
			}
          
			GameLayer->SetRenderOpacity(DesiredOpacity);
		}

		if (UCommonActivatableWidgetContainerBase* GameWindowLayer = RootLayout->GetLayerWidget(WoWUITags::Layer::GameWindow))
		{
			GameWindowLayer->SetRenderOpacity(DesiredOpacity);
		}

		UE_LOG(LogWoWUI, Display, TEXT("Changing opacity of HUD elements in layer Game and Game to %f"), DesiredOpacity);
	}
}
