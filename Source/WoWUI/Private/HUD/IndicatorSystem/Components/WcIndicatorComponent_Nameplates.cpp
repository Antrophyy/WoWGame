#include "HUD/IndicatorSystem/Components/WcIndicatorComponent_Nameplates.h"
#include "Characters/WcNonPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "IndicatorSystem/IndicatorDescriptor.h"
#include "IndicatorSystem/IndicatorManagerComponent.h"

void UWcIndicatorComponent_Nameplates::NativeComponentInitialized()
{
	Super::NativeComponentInitialized();

	UActorComponent* ActorComponent = OwningPlayer->AddComponentByClass(USphereComponent::StaticClass(), true, FTransform(), false);
	USphereComponent* SphereComponent = Cast<USphereComponent>(ActorComponent);
	SphereComponent->SetSphereRadius(5'000.f);
	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleNameplateVisibilityBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::HandleNameplateVisibilityEndOverlap);

	TArray<AActor*> OverlappingActors;
	SphereComponent->GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == OwningPlayer->GetPawn())
			continue;

		HandleNameplateVisibilityBeginOverlap(nullptr, OverlappingActor, nullptr, 0, false, FHitResult());
	}
}

void UWcIndicatorComponent_Nameplates::HandleNameplateVisibilityBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int, bool, const FHitResult&)
{
	const bool bIsTrackablePawn = (bool)Cast<AWcNonPlayerCharacter>(OtherActor);
	const bool bIsAlreadyTracked = IndicatorOwnersMap.Contains(OtherActor->GetActorLabel());

	if (!bIsTrackablePawn || bIsAlreadyTracked)
		return;

	if (UIndicatorManagerComponent* IndicatorManager = UIndicatorManagerComponent::GetComponent(OwningPlayer.Get()))
	{
		UIndicatorDescriptor* Descriptor = NewObject<UIndicatorDescriptor>(this);
		Descriptor->SetDataObject(OtherActor);
		Descriptor->SetSceneComponent(OtherActor->FindComponentByClass<USkeletalMeshComponent>());
		Descriptor->SetProjectionMode(EActorCanvasProjectionMode::ComponentBoundingBox);
		Descriptor->SetIndicatorClass(IndicatorWidgetClass);
		Descriptor->SetPriority(1);
		Descriptor->SetHAlign(HAlign_Center);
		Descriptor->SetVAlign(VAlign_Bottom);
		Descriptor->SetBoundingBoxAnchor(FVector(0.5f, 0.5f, 1.1f));
		Descriptor->SetIsHitTestable(true);

		IndicatorManager->AddIndicator(Descriptor);
		IndicatorOwnersMap.Emplace(OtherActor->GetActorLabel(), Descriptor);
	}
}

void UWcIndicatorComponent_Nameplates::HandleNameplateVisibilityEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int)
{
	if (!IndicatorOwnersMap.Contains(OtherActor->GetActorLabel()))
		return;

	if (UIndicatorManagerComponent* IndicatorManager = UIndicatorManagerComponent::GetComponent(OwningPlayer.Get()))
	{
		IndicatorManager->RemoveIndicator(IndicatorOwnersMap.FindAndRemoveChecked(OtherActor->GetActorLabel()));
	}
}
