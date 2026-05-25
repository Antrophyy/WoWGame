#pragma once

#include "Components/ActorComponent.h"
#include "GameIndicatorManagerComponent.generated.h"

class AController;
class UGameIndicatorDescriptor;

/**
 * UGameIndicatorManagerComponent is responsible for managing game indicators.
 * It allows adding and removing indicators, and notifies listeners about these changes.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=UI)
class GAMEINDICATORSYSTEM_API UGameIndicatorManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGameIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UGameIndicatorManagerComponent* GetComponent(const AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void AddIndicator(UGameIndicatorDescriptor* IndicatorDescriptor);
	
	UFUNCTION(BlueprintCallable, Category = Indicator)
	void RemoveIndicator(UGameIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(UGameIndicatorManagerComponent, FIndicatorEvent, UGameIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	UPROPERTY()
	TArray<TObjectPtr<UGameIndicatorDescriptor>> Indicators;
};
