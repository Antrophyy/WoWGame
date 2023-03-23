#include "Characters/WcNonPlayerCharacter.h"
#include "GameNativeTags.h"
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
	AggroRangeCollision->SetSphereRadius(400.f);
	AggroRangeCollision->SetupAttachment(RootComponent);
}

void AWcNonPlayerCharacter::BroadcastAggroStateChanged()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

	FEnemySpawnedMessage Message;
	Message.Pawn = this;
	Message.bHasAggro = bHasAggro;
	MessageSubsystem.BroadcastMessage(TAG_GAME_EVENTS_ENEMY_SPAWNED, Message);
}

void AWcNonPlayerCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bHasAggro)
		return;

	bHasAggro = true;
	BroadcastAggroStateChanged();
}

void AWcNonPlayerCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!bHasAggro)
		return;

	bHasAggro = false;
	BroadcastAggroStateChanged();
}
