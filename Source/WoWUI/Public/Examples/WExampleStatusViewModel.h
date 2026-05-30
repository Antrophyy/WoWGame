#pragma once

#include "EazyUIMVVM/Public/Core/EazyViewModelBase.h"
#include "TimerManager.h"
#include "WExampleStatusViewModel.generated.h"

UCLASS(BlueprintType)
class WOWUI_API UWExampleStatusViewModel : public UEazyViewModelBase
{
	GENERATED_BODY()
	
public:
	UWExampleStatusViewModel();

protected:
	virtual void OnCreated() override;
	virtual void OnDestroyed() override;

private:
	void HandleDelayedUpdate();

private: // Bindings
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, meta=(AllowPrivateAccess="true"))
	FText PlayerControllerNameText;
	const FText& GetPlayerControllerNameText() const { return PlayerControllerNameText; }
	void SetPlayerControllerNameText(const FText& InPlayerControllerNameText) { VM_SET(PlayerControllerNameText); }

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, meta=(AllowPrivateAccess="true"))
	FText PlayerPawnNameText;
	const FText& GetPlayerPawnNameText() const { return PlayerPawnNameText; }
	void SetPlayerPawnNameText(const FText& InPlayerPawnNameText) { VM_SET(PlayerPawnNameText); }

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, meta=(AllowPrivateAccess="true"))
	FText PlayerStateNameText;
	const FText& GetPlayerStateNameText() const { return PlayerStateNameText; }
	void SetPlayerStateNameText(const FText& InPlayerStateNameText) { VM_SET(PlayerStateNameText); }

private:
	FTimerHandle DelayedUpdateTimerHandle;
};
