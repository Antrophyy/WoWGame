#pragma once

#include "GameFramework/Pawn.h"
#include "WcNonPlayerCharacter.generated.h"

class UCapsuleComponent;
class USceneComponent;
class USkeletalMeshComponent;
class USphereComponent;

UCLASS()
class WOWGAME_API AWcNonPlayerCharacter : public APawn
{
	GENERATED_BODY()

public:

	AWcNonPlayerCharacter();

protected: // Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> AggroRangeCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> HitBoxComponent;

private:

	bool bHasAggro = false;
};
