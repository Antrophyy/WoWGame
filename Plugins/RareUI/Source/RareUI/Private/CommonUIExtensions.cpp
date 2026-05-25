
#include "CommonUIExtensions.h"
#include "RareGameUIManagerSubsystem.h"
#include "LogRareUI.h"
#include "RarePrimaryGameLayout.h"
#include "CommonInputSubsystem.h"
#include "CommonUITypes.h"
#include "EnhancedInputSubsystems.h"
#include "RareGameUIPolicy.h"
#include "Styling/StyleDefaults.h"

int32 UCommonUIExtensions::InputSuspensions = 0;

ECommonInputType UCommonUIExtensions::GetOwningPlayerInputType(const UUserWidget* WidgetContextObject)
{
	if (IsValid(WidgetContextObject))
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
	if (IsValid(WidgetContextObject))
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
	if (IsValid(WidgetContextObject))
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
	if (const URareGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<URareGameUIManagerSubsystem>())
	{
		if (const URareGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy())
		{
			if (URarePrimaryGameLayout* RootLayout = Policy->GetRootLayout(LocalPlayer))
			{
				return RootLayout->PushWidgetToLayer(LayerName, WidgetClass);
			}
		}
	}

	return nullptr;
}

void UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, const FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (const URareGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<URareGameUIManagerSubsystem>())
	{
		if (const URareGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy())
		{
			if (URarePrimaryGameLayout* RootLayout = Policy->GetRootLayout(LocalPlayer))
			{
				constexpr bool bSuspendInputUntilComplete = true;
				RootLayout->PushWidgetToLayerAsync(LayerName, bSuspendInputUntilComplete, WidgetClass);
			}
		}
	}
}

void UCommonUIExtensions::PopContentFromLayer(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag LayerName)
{
	if (!IsValid(ActivatableWidget))
	{
		UE_LOG(LogRareUI, Warning, TEXT("[%s::%hs] -> ActivatableWidget is invalid."), *StaticClass()->GetName(), __func__);
		return;
	}

	if (const ULocalPlayer* LocalPlayer = ActivatableWidget->GetOwningLocalPlayer())
	{
		if (const URareGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<URareGameUIManagerSubsystem>())
		{
			if (const URareGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy())
			{
				if (const URarePrimaryGameLayout* RootLayout = Policy->GetRootLayout(LocalPlayer))
				{
					RootLayout->FindAndRemoveWidgetFromLayer(ActivatableWidget, LayerName);
				}
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

FKey UCommonUIExtensions::GetKeyForInputType(const ULocalPlayer* LocalPlayer, ECommonInputType InputType, const UInputAction* InputAction, const int32 KeyIndex)
{
	if (!LocalPlayer)
	{
		return FKey();
	}

	TArray<FKey> Keys;
	if (const UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Keys = EnhancedInputLocalPlayerSubsystem->QueryKeysMappedToAction(InputAction);
	}
	
	Keys = Keys.FilterByPredicate([InputType](const FKey& Key)
	{
		return CommonUI::IsKeyValidForInputType(Key, InputType);
	});
	
	return Keys.IsValidIndex(KeyIndex) ? Keys[KeyIndex] : FKey();
}

FSlateBrush UCommonUIExtensions::GetIconForInputAction(const UCommonInputSubsystem* CommonInputSubsystem, const UInputAction* InputAction, const int32 KeyIndex)
{
	const FKey FirstKeyForCurrentInput = GetKeyForInputType(CommonInputSubsystem->GetLocalPlayer(), CommonInputSubsystem->GetCurrentInputType(), InputAction, KeyIndex);

	FSlateBrush SlateBrush;
	if (FirstKeyForCurrentInput.IsValid() && UCommonInputPlatformSettings::Get()->TryGetInputBrush(SlateBrush, FirstKeyForCurrentInput, CommonInputSubsystem->GetCurrentInputType(), CommonInputSubsystem->GetCurrentGamepadName()))
	{
		return SlateBrush;
	}

	return *FStyleDefaults::GetNoBrush();
}
