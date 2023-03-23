#pragma once

#include "GameFramework/Pawn.h"
#include "WcNonPlayerCharacter.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class USphereComponent;

USTRUCT(BlueprintType)
struct FEnemySpawnedMessage
{
	GENERATED_BODY()

	TWeakObjectPtr<APawn> Pawn;

	bool bHasAggro = false;
};

UCLASS()
class WOWGAME_API AWcNonPlayerCharacter : public APawn
{
	GENERATED_BODY()

public:

	AWcNonPlayerCharacter();

protected: // Overriden Functions

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected: // Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> AggroRangeCollision;

private:

	void BroadcastAggroStateChanged();

	bool bHasAggro = false;
};
