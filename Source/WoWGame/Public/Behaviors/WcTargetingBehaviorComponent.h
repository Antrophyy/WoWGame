#pragma once

#include "Components/ActorComponent.h"
#include "WcTargetingBehaviorComponent.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FTargetSelectedMessage
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> CurrentTarget;

	TWeakObjectPtr<AActor> PreviousTarget;
};

UCLASS()
class WOWGAME_API UWcTargetingBehaviorComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	void ChangeTarget(AActor* NewTarget);

	TWeakObjectPtr<AActor> GetCurrentTarget() const { return CurrentTarget; }

	TWeakObjectPtr<AActor> GetPreviousTarget() const { return PreviousTarget; }

private:

	void BroadcastTargetSelected() const;

	TWeakObjectPtr<AActor> PreviousTarget;

	TWeakObjectPtr<AActor> CurrentTarget;
};
