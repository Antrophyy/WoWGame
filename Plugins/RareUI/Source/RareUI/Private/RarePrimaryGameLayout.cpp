#include "RarePrimaryGameLayout.h"

#if WITH_EDITOR
#include "Editor.h"
#endif // WITH_EDITOR
#include "RareGameUIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LogRareUI.h"
#include "RareGameUIPolicy.h"
#include "Foundation/RareCommonActivatableWidgetStack.h"

URarePrimaryGameLayout* URarePrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	const APlayerController* PlayerController = GameInstance ? GameInstance->GetPrimaryPlayerController(false) : nullptr;
	return GetPrimaryGameLayout(PlayerController);
}

URarePrimaryGameLayout* URarePrimaryGameLayout::GetPrimaryGameLayout(const APlayerController* PlayerController)
{
	return PlayerController ? GetPrimaryGameLayout(Cast<ULocalPlayer>(PlayerController->Player)) : nullptr;
}

URarePrimaryGameLayout* URarePrimaryGameLayout::GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer)
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
			const URareGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<URareGameUIManagerSubsystem>();
			if (ensureAlwaysMsgf(IsValid(UIManager), TEXT("[GetPrimaryGameLayout] UI Manager Subsystem is NULL. Check if the file is created and inherits from RareGameUIManagerSubsystem. Or check that it's not marked as Abstract.")))
			{
				const URareGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy();
				
				if (!IsValid(Policy))
				{
					ensureAlwaysMsgf(false, TEXT("[GetPrimaryGameLayout] UIPolicy is null, meaning you probably didn't put it into the DefaultEngine.ini."));
					return nullptr;
				}
				
				URarePrimaryGameLayout* RootLayout = Policy->GetRootLayout(LocalPlayer);
				if (ensureAlwaysMsgf(IsValid(RootLayout), TEXT("[GetPrimaryGameLayout] Root Layout is NULL, Wrong Player Controller is selected in Game Mode OR Wrong Local Player inside Project Settings ")))
				{
					return RootLayout;
				}
			}
		}
	}

	return nullptr;
}

URarePrimaryGameLayout::URarePrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

void URarePrimaryGameLayout::RegisterLayer(const FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &URarePrimaryGameLayout::OnLayerTransitioning);

		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void URarePrimaryGameLayout::OnLayerTransitioning(UCommonActivatableWidgetContainerBase* Widget, const bool IsTransitioning)
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

void URarePrimaryGameLayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName) const
{
	UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName);
	if (ensureAlwaysMsgf(IsValid(Container), TEXT("[FindAndRemoveWidgetFromLayer] Attempting to remove widget from nonexistent layer")))
	{
		UE_LOG(LogRareUI, Display, TEXT("Removing widget with name %s from stack %s"), *ActivatableWidget->GetName(), *LayerName.ToString());
		Container->RemoveWidget(*ActivatableWidget);
	}
}

void URarePrimaryGameLayout::ClearWidgetsFromLayer(const FGameplayTag LayerName) const
{
	if (UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		UE_LOG(LogRareUI, Display, TEXT("Clearing all widgets from stack %s"), *LayerName.ToString());
		Container->ClearWidgets();
	}
}

URareCommonActivatableWidgetStack* URarePrimaryGameLayout::GetOwningContainerStack(const UCommonActivatableWidget* InActivatableWidget) const
{
	TArray<UCommonActivatableWidgetContainerBase*> Containers;
	Layers.GenerateValueArray(Containers);

	for (UCommonActivatableWidgetContainerBase* Container : Containers)
	{
		if (URareCommonActivatableWidgetStack* ContainerStack = Cast<URareCommonActivatableWidgetStack>(Container))
		{
			for (const UUserWidget* CurrentlyActiveWidget : ContainerStack->GetGeneratedWidgetsPool().GetActiveWidgets())
			{
				if (CurrentlyActiveWidget == InActivatableWidget)
				{
					return Cast<URareCommonActivatableWidgetStack>(Container);
				}
			}
		}
	}
	
	return nullptr;
}

bool URarePrimaryGameLayout::IsLayerClear(const FGameplayTag LayerName) const
{
	if (const UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(LayerName))
	{
		return Container->GetNumWidgets() <= 0;
	}
	else
	{
		return true;
	}
}

UCommonActivatableWidgetContainerBase* URarePrimaryGameLayout::GetLayerWidget(const FGameplayTag LayerName) const
{
	return Layers.FindRef(LayerName);
}

void URarePrimaryGameLayout::SetIsDormant(const bool bInDormant)
{
	if (bIsDormant != bInDormant)
	{
		const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
		const int32 PlayerId = LocalPlayer ? LocalPlayer->GetControllerId() : -1;
		const TCHAR* OldDormancyStr = bIsDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* NewDormancyStr = bInDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* PrimaryPlayerStr = LocalPlayer && LocalPlayer->IsPrimaryPlayer() ? TEXT("[Primary]") : TEXT("[Non-Primary]");
		UE_LOG(LogRareUI, Display, TEXT("%s PrimaryGameLayout Dormancy changed for [%d] from [%s] to [%s]"), PrimaryPlayerStr, PlayerId,
			   OldDormancyStr, NewDormancyStr);

		bIsDormant = bInDormant;
		OnIsDormantChanged();
	}
}

void URarePrimaryGameLayout::OnIsDormantChanged() {}
