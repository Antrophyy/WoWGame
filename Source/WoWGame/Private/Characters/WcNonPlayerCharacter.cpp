#include "Characters/WcNonPlayerCharacter.h"
#include "GameNativeTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

AWcNonPlayerCharacter::AWcNonPlayerCharacter()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	AggroRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("AggroRangeCollision"));
	AggroRangeCollision->SetupAttachment(RootComponent);

	HitBoxComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBoxComponent"));
	HitBoxComponent->SetupAttachment(RootComponent);
}
