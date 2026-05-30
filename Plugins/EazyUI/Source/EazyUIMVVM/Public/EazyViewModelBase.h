#pragma once

#include "MVVMViewModelBase.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "EazyViewModelBase.generated.h"

#define VM_SET_1(PropertyName) \
	UE_MVVM_SET_PROPERTY_VALUE(PropertyName, In##PropertyName)
#define VM_SET_2(PropertyName, NewValue) \
	UE_MVVM_SET_PROPERTY_VALUE(PropertyName, NewValue)
#define VM_SET_GET_MACRO(_1, _2, NAME, ...) NAME
#define VM_SET(...) VM_SET_GET_MACRO(__VA_ARGS__, VM_SET_2, VM_SET_1)(__VA_ARGS__)

UENUM(Category=UI, BlueprintType)
enum EEazyViewModelLifetimePolicy : uint8
{
	// The subsystem will try to reuse an existing VM if possible. A new instance will only be created if there is no existing instance that can be reused (after all users died).
	Default,
	// A new instance is always created when requested. The subsystem will never try to reuse an existing instance. VM will die with its owner.
	AlwaysCreateNewInstance,
	// After the first instance is created, it will be reused for all subsequent requests. The owner is the MVVM subsystem and will never die.
	Persistent,
};

UCLASS(Abstract)
class EAZYUIMVVM_API UEazyViewModelBase : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void InitializeViewModel(ULocalPlayer* InLocalPlayer);
	void DeinitializeViewModel();
	
	// Can be overriden to specify a different lifetime policy for this VM.
	virtual EEazyViewModelLifetimePolicy GetDesiredLifetimePolicy() const { return LifetimePolicy; }
	
	FSimpleMulticastDelegate OnDestroyedEvent;
	FSimpleMulticastDelegate OnCreatedEvent;

protected:
	virtual void OnCreated();
	virtual void OnDestroyed();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="OnCreated")
	void BP_OnCreated();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="OnDestroyed")
	void BP_OnDestroyed();
	
protected: // Helper functions.

	template <class TLocalPlayer = ULocalPlayer>
	TLocalPlayer* GetOwningLocalPlayer() const
	{
		return Cast<TLocalPlayer>(OwningLocalPlayer.Get());
	}
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="EazyUI|ViewModel", DisplayName="Get Owning Local Player")
	ULocalPlayer* BP_GetOwningLocalPlayer() const;

	template <class TPlayerController = APlayerController>
	TPlayerController* GetOwningPlayer() const
	{
		return OwningLocalPlayer.IsValid() ? Cast<TPlayerController>(OwningLocalPlayer->GetPlayerController(GetWorld())) : nullptr;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="EazyUI|ViewModel", DisplayName="Get Owning Player Controller")
	APlayerController* BP_GetOwningPlayer() const;

	template <class TPawn = APawn>
	TPawn* GetOwningPlayerPawn() const
	{
		return GetOwningPlayer() ? Cast<TPawn>(GetOwningPlayer()->GetPawn()) : nullptr;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="EazyUI|ViewModel", DisplayName="Get Owning Player Pawn")
	APawn* BP_GetOwningPlayerPawn() const;

	template <class TPlayerState = APlayerState>
	TPlayerState* GetOwningPlayerState() const
	{
		return GetOwningPlayer() ? Cast<TPlayerState>(GetOwningPlayer()->PlayerState) : nullptr;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="EazyUI|ViewModel", DisplayName="Get Owning Player State")
	APlayerState* BP_GetOwningPlayerState() const;

private:
	
	UPROPERTY(EditAnywhere, Category="EazyUI", meta=(AllowPrivateAccess=true))
	TEnumAsByte<EEazyViewModelLifetimePolicy> LifetimePolicy = EEazyViewModelLifetimePolicy::Default;
	
	TWeakObjectPtr<ULocalPlayer> OwningLocalPlayer;
};

