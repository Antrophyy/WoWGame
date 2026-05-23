// Copyright (C) Grip Studios. All Rights Reserved

#include "GameIndicatorManagerComponent.h"

#include "GameIndicatorDescriptor.h"
#include "GameFramework/Controller.h"

UGameIndicatorManagerComponent::UGameIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

UGameIndicatorManagerComponent* UGameIndicatorManagerComponent::GetComponent(const AController* Controller)
{
	return Controller ? Controller->FindComponentByClass<UGameIndicatorManagerComponent>() : nullptr;
}

void UGameIndicatorManagerComponent::AddIndicator(UGameIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->ManagerComponent = this;
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UGameIndicatorManagerComponent::RemoveIndicator(UGameIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->ManagerComponent == this);
	
		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
