#pragma once

#include "CommonActivatableWidget.h"
#include "CommonInputBaseTypes.h"
#include "GameplayTagContainer.h"
#include "CommonUIExtensions.generated.h"

class UInputAction;

UCLASS(ClassGroup=UI)
class UIEXTENSIONS_API UCommonUIExtensions final : public UObject
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
	static UCommonActivatableWidget* PushContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer,
	                                                              UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName,
	                                                              UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void PushStreamedContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer,
	                                                 UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName,
	                                                 UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void PopContentFromLayer(UPARAM(meta = (AllowAbstract = false)) UCommonActivatableWidget* ActivatableWidget,
	                                UPARAM(meta = (Categories = "UI.Layer")) const FGameplayTag LayerName);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static ULocalPlayer* GetLocalPlayerFromController(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static FName SuspendInputForPlayer(APlayerController* PlayerController, FName SuspendReason);

	static FName SuspendInputForPlayer(const ULocalPlayer* LocalPlayer, FName SuspendReason);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void ResumeInputForPlayer(APlayerController* PlayerController, const FName SuspendToken);

	static void ResumeInputForPlayer(const ULocalPlayer* LocalPlayer, const FName SuspendToken);
	
	static FSlateBrush GetIconForInputAction(const UCommonInputSubsystem* InputSubsystem, const UInputAction* InputAction);

private:
	static int32 InputSuspensions;
};
