#pragma once

#include "Components/ControllerComponent.h"
#include "IndicatorManagerComponent.generated.h"

class UIndicatorDescriptor;

UCLASS(BlueprintType, Blueprintable)
class UIEXTENSIONS_API UIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:

	UIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UIndicatorManagerComponent* GetComponent(const AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(UIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)

	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};
