#include "Game/HUD/IndicatorSystem/WIndicatorLayerWidget.h"

#include "GameIndicatorManagerComponent.h"
#include "Game/HUD/IndicatorSystem/Handlers/WIndicatorHandler_Nameplate.h"

void UWIndicatorLayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GetOwningPlayer()->AddComponentByClass(UGameIndicatorManagerComponent::StaticClass(), true, FTransform(), true);

	CreateComponents();
}

void UWIndicatorLayerWidget::CreateComponents()
{
	IndicatorHandler_Nameplates = NewObject<UWIndicatorHandler_Nameplate>(this);
	IndicatorHandler_Nameplates->InitializeComponent(GetOwningPlayer(), NameplateWidget_Class);
}
