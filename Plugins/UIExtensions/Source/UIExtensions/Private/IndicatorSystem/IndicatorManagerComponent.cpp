﻿#include "IndicatorSystem/IndicatorManagerComponent.h"
#include "IndicatorSystem/IndicatorDescriptor.h"

UIndicatorManagerComponent::UIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

UIndicatorManagerComponent* UIndicatorManagerComponent::GetComponent(const AController* Controller)
{
	return Controller ? Controller->FindComponentByClass<UIndicatorManagerComponent>() : nullptr;
}

void UIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->ManagerComponent = this;
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->ManagerComponent == this);

		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
	}
}
