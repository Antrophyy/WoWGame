#include "CommonUIExtensions.h"
#include "GameUIManagerSubsystem.h"
#include "LogUIExtensions.h"
#include "PrimaryGameLayout.h"
#include "CommonInputSubsystem.h"

int32 UCommonUIExtensions::InputSuspensions = 0;

ECommonInputType UCommonUIExtensions::GetOwningPlayerInputType(const UUserWidget* WidgetContextObject)
{
	if (WidgetContextObject)
	{
		if (const UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(WidgetContextObject->GetOwningLocalPlayer()))
		{
			return InputSubsystem->GetCurrentInputType();
		}
	}

	return ECommonInputType::Count;
}

bool UCommonUIExtensions::IsOwningPlayerUsingTouch(const UUserWidget* WidgetContextObject)
{
	if (WidgetContextObject)
	{
		if (const UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(WidgetContextObject->GetOwningLocalPlayer()))
		{
			return InputSubsystem->GetCurrentInputType() == ECommonInputType::Touch;
		}
	}
	return false;
}

bool UCommonUIExtensions::IsOwningPlayerUsingGamepad(const UUserWidget* WidgetContextObject)
{
	if (WidgetContextObject)
	{
		if (const UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(WidgetContextObject->GetOwningLocalPlayer()))
		{
			return InputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad;
		}
	}
	return false;
}

UCommonActivatableWidget* UCommonUIExtensions::PushContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, const FGameplayTag LayerName, const TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (const UGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>())
	{
		if (UPrimaryGameLayout* RootLayout = UIManager->GetRootLayout(LocalPlayer))
		{
			return RootLayout->PushWidgetToLayer(LayerName, WidgetClass);
		}
	}

	return nullptr;
}

void UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, const FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (const UGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>())
	{
		if (UPrimaryGameLayout* RootLayout = UIManager->GetRootLayout(LocalPlayer))
		{
			constexpr bool bSuspendInputUntilComplete = true;
			RootLayout->PushWidgetToLayerAsync(LayerName, bSuspendInputUntilComplete, WidgetClass);
		}
	}
}

void UCommonUIExtensions::PopContentFromLayer(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName)
{
	if (!IsValid(ActivatableWidget))
	{
		UE_LOG(LogUIExtensions, Warning, TEXT("[%s::%hs] -> ActivatableWidget is invalid."), *StaticClass()->GetName(), __func__);
		return;
	}

	if (const ULocalPlayer* LocalPlayer = ActivatableWidget->GetOwningLocalPlayer())
	{
		if (const UGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>())
		{
			if (const UPrimaryGameLayout* RootLayout = UIManager->GetRootLayout(LocalPlayer))
			{
				RootLayout->FindAndRemoveWidgetFromLayer(ActivatableWidget, LayerName);
			}
		}
	}
}

ULocalPlayer* UCommonUIExtensions::GetLocalPlayerFromController(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		return Cast<ULocalPlayer>(PlayerController->Player);
	}

	return nullptr;
}

FName UCommonUIExtensions::SuspendInputForPlayer(APlayerController* PlayerController, const FName SuspendReason)
{
	return SuspendInputForPlayer(PlayerController ? PlayerController->GetLocalPlayer() : nullptr, SuspendReason);
}

FName UCommonUIExtensions::SuspendInputForPlayer(const ULocalPlayer* LocalPlayer, const FName SuspendReason)
{
	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		InputSuspensions++;
		FName SuspendToken = SuspendReason;
		SuspendToken.SetNumber(InputSuspensions);

		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, true);

		return SuspendToken;
	}

	return NAME_None;
}

void UCommonUIExtensions::ResumeInputForPlayer(APlayerController* PlayerController, const FName SuspendToken)
{
	ResumeInputForPlayer(PlayerController ? PlayerController->GetLocalPlayer() : nullptr, SuspendToken);
}

void UCommonUIExtensions::ResumeInputForPlayer(const ULocalPlayer* LocalPlayer, const FName SuspendToken)
{
	if (SuspendToken == NAME_None)
	{
		return;
	}

	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, false);
	}
}
