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

UCLASS(Abstract)
class EAZYUIMVVM_API UEazyViewModelBase : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	void InitializeViewModel(ULocalPlayer* InLocalPlayer);
	void DeinitializeViewModel();
	
	bool IsPersistent() const { return bIsPersistent; }
	bool AlwaysCreateNewInstance() const { return bAlwaysCreateNewInstance; }
	
	FSimpleMulticastDelegate OnDestroyedEvent;
	FSimpleMulticastDelegate OnCreatedEvent;

protected:
	virtual void OnCreated();
	virtual void OnDestroyed();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="OnCreated")
	void BP_OnCreated();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="OnDestroyed")
	void BP_OnDestroyed();
	
protected: // Set by the Editor
	
	// If set to true, a new instance is always created when requested. If false, the subsystem will try to reuse an existing instance if possible.
	UPROPERTY(EditAnywhere, Category="EazyUI|ViewModel", meta=(EditCondition="!bIsPersistent"))
	bool bAlwaysCreateNewInstance = false;
	
	// If set to true, once the viewmodel is first created, it will never be destroyed and will be re-used. AllowMultipleInstances will be ignored if this is true.
	UPROPERTY(EditAnywhere, Category="EazyUI|ViewModel")
	bool bIsPersistent = false;

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
	TWeakObjectPtr<ULocalPlayer> OwningLocalPlayer;
};

