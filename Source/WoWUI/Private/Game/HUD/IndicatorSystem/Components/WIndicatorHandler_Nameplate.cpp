#include "Game/HUD/IndicatorSystem/Handlers/WIndicatorHandler_Nameplate.h"

#include "GameIndicatorDescriptor.h"
#include "GameIndicatorManagerComponent.h"
#include "Characters/WcNonPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"

void UWIndicatorHandler_Nameplate::NativeComponentInitialized()
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

void UWIndicatorHandler_Nameplate::HandleNameplateVisibilityBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int, bool, const FHitResult&)
{
	const bool bIsTrackablePawn = (bool)Cast<AWcNonPlayerCharacter>(OtherActor);
	const bool bIsAlreadyTracked = IndicatorOwnersMap.Contains(OtherActor->GetName());

	if (!bIsTrackablePawn || bIsAlreadyTracked)
		return;

	if (UGameIndicatorManagerComponent* IndicatorManager = UGameIndicatorManagerComponent::GetComponent(OwningPlayer.Get()))
	{
		UGameIndicatorDescriptor* Descriptor = NewObject<UGameIndicatorDescriptor>(this);
		Descriptor->DataObject = OtherActor;
		Descriptor->SceneComponent = OtherActor->FindComponentByClass<USkeletalMeshComponent>();
		Descriptor->ProjectionMode = EActorCanvasProjectionMode::ComponentBoundingBox;
		Descriptor->IndicatorWidgetClass = IndicatorWidgetClass;
		Descriptor->Priority = 1;
		Descriptor->HAlignment = HAlign_Center;
		Descriptor->VAlignment = VAlign_Bottom;
		Descriptor->BoundingBoxAnchor = FVector(0.5f, 0.5f, 1.1f);
		Descriptor->bIsHitTestable = true;

		IndicatorManager->AddIndicator(Descriptor);
		IndicatorOwnersMap.Emplace(OtherActor->GetName(), Descriptor);
	}
}

void UWIndicatorHandler_Nameplate::HandleNameplateVisibilityEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int)
{
	if (!IndicatorOwnersMap.Contains(OtherActor->GetName()))
		return;

	if (UGameIndicatorManagerComponent* IndicatorManager = UGameIndicatorManagerComponent::GetComponent(OwningPlayer.Get()))
	{
		IndicatorManager->RemoveIndicator(IndicatorOwnersMap.FindAndRemoveChecked(OtherActor->GetName()).ResolveObjectPtr());
	}
}
