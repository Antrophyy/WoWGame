#include "EazyPrimaryGameLayout.h"

#if WITH_EDITOR
#include "Editor.h"
#endif // WITH_EDITOR
#include "EazyGameUIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LogEazyUI.h"
#include "EazyGameUIPolicy.h"

UEazyPrimaryGameLayout* UEazyPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	const APlayerController* PlayerController = GameInstance ? GameInstance->GetPrimaryPlayerController(false) : nullptr;
	return GetPrimaryGameLayout(PlayerController);
}

UEazyPrimaryGameLayout* UEazyPrimaryGameLayout::GetPrimaryGameLayout(const APlayerController* PlayerController)
{
	return PlayerController ? GetPrimaryGameLayout(Cast<ULocalPlayer>(PlayerController->Player)) : nullptr;
}

UEazyPrimaryGameLayout* UEazyPrimaryGameLayout::GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer)
{
#if WITH_EDITOR
	if (GEditor && GEditor->IsSimulateInEditorInProgress())
	{
		return nullptr;
	}
#endif // WITH_EDITOR

	if (IsValid(LocalPlayer))
	{
		if (const UGameInstance* GameInstance = LocalPlayer->GetGameInstance())
		{
			const UEazyGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UEazyGameUIManagerSubsystem>();
			if (ensureAlwaysMsgf(IsValid(UIManager), TEXT("[GetPrimaryGameLayout] UI Manager Subsystem is NULL. Check if the file is created and inherits from EazyGameUIManagerSubsystem. Or check that it's not marked as Abstract.")))
			{
				const UEazyGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy();

				if (!IsValid(Policy))
				{
					ensureAlwaysMsgf(false, TEXT("[GetPrimaryGameLayout] UIPolicy is null, meaning you probably didn't put it into the DefaultEngine.ini."));
					return nullptr;
				}

				UEazyPrimaryGameLayout* RootLayout = Policy->GetRootLayout(LocalPlayer);
				if (ensureAlwaysMsgf(IsValid(RootLayout), TEXT("[GetPrimaryGameLayout] Root Layout is NULL, Wrong Player Controller is selected in Game Mode OR Wrong Local Player inside Project Settings ")))
				{
					return RootLayout;
				}
			}
		}
	}

	return nullptr;
}

UEazyPrimaryGameLayout::UEazyPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

void UEazyPrimaryGameLayout::RegisterLayer(const FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!ensureMsgf(IsValid(LayerWidget), TEXT("[RegisterLayer] LayerWidget is invalid for layer %s"), *LayerTag.ToString()))
	{
		return;
	}

	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &UEazyPrimaryGameLayout::OnLayerTransitioning);

		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UEazyPrimaryGameLayout::OnLayerTransitioning(UCommonActivatableWidgetContainerBase*, const bool IsTransitioning)
{
	if (IsTransitioning)
	{
		const FName SuspendToken = UCommonUIExtensions::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransition"));
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

void UEazyPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UEazyCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName) const
{
	if (!ensureMsgf(IsValid(ActivatableWidget), TEXT("[FindAndRemoveWidgetFromLayer] ActivatableWidget is invalid")))
	{
		return;
	}

	UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName);
	if (ensureMsgf(IsValid(Container), TEXT("[FindAndRemoveWidgetFromLayer] Attempting to remove widget from nonexistent layer")))
	{
		UE_LOG(LogEazyUI, Display, TEXT("Removing widget with name %s from stack %s"), *ActivatableWidget->GetName(), *LayerName.ToString());
		Container->RemoveWidget(*ActivatableWidget);
	}
}

void UEazyPrimaryGameLayout::ClearWidgetsFromLayer(const FGameplayTag LayerName) const
{
	if (UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		UE_LOG(LogEazyUI, Display, TEXT("Clearing all widgets from stack %s"), *LayerName.ToString());
		Container->ClearWidgets();
	}
}

bool UEazyPrimaryGameLayout::IsLayerClear(const FGameplayTag LayerName) const
{
	if (const UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		return Container->GetNumWidgets() == 0;
	}

	return true;
}

UCommonActivatableWidgetContainerBase* UEazyPrimaryGameLayout::GetLayerWidget(const FGameplayTag LayerName) const
{
	return Layers.FindRef(LayerName);
}

void UEazyPrimaryGameLayout::SetIsDormant(const bool bInDormant)
{
	if (bIsDormant != bInDormant)
	{
		const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		const int32 PlayerId = LocalPlayer ? LocalPlayer->GetControllerId() : -1;
		const TCHAR* OldDormancyStr = bIsDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* NewDormancyStr = bInDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* PrimaryPlayerStr = LocalPlayer && LocalPlayer->IsPrimaryPlayer() ? TEXT("[Primary]") : TEXT("[Non-Primary]");
		UE_LOG(LogEazyUI, Display, TEXT("%s PrimaryGameLayout Dormancy changed for [%d] from [%s] to [%s]"), PrimaryPlayerStr, PlayerId,
		       OldDormancyStr, NewDormancyStr);

		bIsDormant = bInDormant;
		OnIsDormantChanged();
	}
}

void UEazyPrimaryGameLayout::OnIsDormantChanged() {}
