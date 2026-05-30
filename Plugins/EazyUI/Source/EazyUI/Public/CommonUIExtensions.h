#pragma once

#include "CommonInputBaseTypes.h"
#include "GameplayTagContainer.h"
#include "Foundation/EazyCommonActivatableWidget.h"
#include "CommonUIExtensions.generated.h"

class UInputAction;

UCLASS(ClassGroup=UI)
class EAZYUI_API UCommonUIExtensions : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Global UI Extensions", meta = (WorldContext = "WidgetContextObject"))
	static ECommonInputType GetOwningPlayerInputType(const UUserWidget* WidgetContextObject);

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Global UI Extensions", meta = (WorldContext = "WidgetContextObject"))
	static bool IsOwningPlayerUsingTouch(const UUserWidget* WidgetContextObject);

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Global UI Extensions", meta = (WorldContext = "WidgetContextObject"))
	static bool IsOwningPlayerUsingGamepad(const UUserWidget* WidgetContextObject);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void PushWidgetToLayerAsync(const ULocalPlayer* LocalPlayer,
	                                   UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName,
	                                   UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UEazyCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void PopContentFromLayer(UPARAM(meta = (AllowAbstract = false)) UEazyCommonActivatableWidget* ActivatableWidget,
	                                UPARAM(meta = (Categories = "UI.Layer")) const FGameplayTag LayerName);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static ULocalPlayer* GetLocalPlayerFromController(APlayerController* PlayerController);

	static FName SuspendInputForPlayer(APlayerController* PlayerController, FName SuspendReason);
	static FName SuspendInputForPlayer(const ULocalPlayer* LocalPlayer, FName SuspendReason);

	static void ResumeInputForPlayer(APlayerController* PlayerController, const FName SuspendToken);
	static void ResumeInputForPlayer(const ULocalPlayer* LocalPlayer, const FName SuspendToken);

	static FKey GetKeyForInputType(const ULocalPlayer* LocalPlayer, ECommonInputType InputType, const UInputAction* InputAction, const int32 KeyIndex);

	static FSlateBrush GetIconForInputAction(const UCommonInputSubsystem* InputSubsystem, const UInputAction* InputAction, const int32 KeyIndex);

private:
	static int32 InputSuspensions;
};
