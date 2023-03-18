#include "PrimaryGameLayout.h"
#include "GameUIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	const APlayerController* PlayerController = GameInstance ? GameInstance->GetPrimaryPlayerController(false) : nullptr;
	return GetPrimaryGameLayout(PlayerController);
}

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(const APlayerController* PlayerController)
{
	return PlayerController ? GetPrimaryGameLayout(Cast<ULocalPlayer>(PlayerController->Player)) : nullptr;
}

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer)
	{
		if (const UGameInstance* GameInstance = LocalPlayer->GetGameInstance())
		{
			if (const UGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UGameUIManagerSubsystem>())
			{
				if (UPrimaryGameLayout* RootLayout = UIManager->GetRootLayout(LocalPlayer))
				{
					return RootLayout;
				}
			}
		}
	}

	return nullptr;
}

UPrimaryGameLayout::UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

void UPrimaryGameLayout::RegisterLayer(const FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &UPrimaryGameLayout::OnLayerTransitioning);

		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UPrimaryGameLayout::OnLayerTransitioning(UCommonActivatableWidgetContainerBase* Widget, const bool IsTransitioning)
{
	if (IsTransitioning)
	{
		const FName SuspendToken = UCommonUIExtensions::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransion"));
		SuspendInputTokens.Add(SuspendToken);
	}
	else
	{
		if (ensure(SuspendInputTokens.Num() > 0))
		{
			const FName SuspendToken = SuspendInputTokens.Pop();
			UCommonUIExtensions::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
		}
	}
}

void UPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName) const
{
	if (UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		Container->RemoveWidget(*ActivatableWidget);
	}
}

void UPrimaryGameLayout::ClearWidgetsFromLayer(const FGameplayTag LayerName) const
{
	if (UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		Container->ClearWidgets();
	}
}

bool UPrimaryGameLayout::IsLayerClear(const FGameplayTag LayerName) const
{
	if (UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		return Container->GetNumWidgets() <= 0;
	}
	else
	{
		return true;
	}
}

UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidget(const FGameplayTag LayerName) const
{
	return Layers.FindRef(LayerName);
}
