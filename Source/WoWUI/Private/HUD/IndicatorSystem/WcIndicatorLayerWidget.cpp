#include "HUD/IndicatorSystem/WcIndicatorLayerWidget.h"
#include "HUD/IndicatorSystem/Components/WcIndicatorComponent_Nameplates.h"
#include "IndicatorSystem/IndicatorManagerComponent.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif // WITH_EDITOR

void UWcIndicatorLayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GetOwningPlayer()->AddComponentByClass(UIndicatorManagerComponent::StaticClass(), true, FTransform(), true);

	CreateComponents();
}

#if WITH_EDITOR
void UWcIndicatorLayerWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (NameplateWidgetClass.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString("{0} has no NameplateWidgetClass specified in the widget defaults."), FText::FromString(GetName())));
	}
}
#endif // WITH_EDITOR

void UWcIndicatorLayerWidget::CreateComponents()
{
	NameplatesComponent = NewObject<UWcIndicatorComponent_Nameplates>(this);
	NameplatesComponent->InitializeComponent(GetOwningPlayer(), NameplateWidgetClass);
}
