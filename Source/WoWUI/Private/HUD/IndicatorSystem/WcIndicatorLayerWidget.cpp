#include "HUD/IndicatorSystem/WcIndicatorLayerWidget.h"
#include "GameNativeTags.h"
#include "LogWoWUI.h"
#include "Characters/WcNonPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Editor/WidgetCompilerLog.h"
#include "IndicatorSystem/IndicatorDescriptor.h"
#include "IndicatorSystem/IndicatorManagerComponent.h"

void UWcIndicatorLayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GetOwningPlayer()->AddComponentByClass(UIndicatorManagerComponent::StaticClass(), true, FTransform(), true);

	RegisterListeners();
}

void UWcIndicatorLayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWcIndicatorLayerWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UnregisterListeners();
}

void UWcIndicatorLayerWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (NameplateWidgetClass.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString("{0} has no NameplateWidgetClass specified in the widget defaults."), FText::FromString(GetName())));
	}
}

void UWcIndicatorLayerWidget::RegisterListeners()
{
	if (!EnemySpawnedListener.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.RegisterListener(TAG_GAME_EVENTS_ENEMY_SPAWNED, this, &ThisClass::HandleEnemyAggroStateChanged);
	}
}

void UWcIndicatorLayerWidget::UnregisterListeners()
{
	if (EnemySpawnedListener.IsValid())
	{
		EnemySpawnedListener.Unregister();
	}
}

void UWcIndicatorLayerWidget::HandleEnemyAggroStateChanged(const FGameplayTag GameplayTag, const FEnemySpawnedMessage& Message)
{
	if (!Message.Pawn.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> The actor to attach a nameplate to is not valid."), *StaticClass()->GetName(), __func__);
		return;
	}

	UIndicatorManagerComponent* IndicatorManager = UIndicatorManagerComponent::GetComponent(GetOwningPlayer());
	if (!IsValid(IndicatorManager))
		return;

	if (Message.bHasAggro)
	{
		if (IndicatorOwnersMap.Contains(Message.Pawn->GetActorLabel()))
			return;

		UIndicatorDescriptor* Descriptor = NewObject<UIndicatorDescriptor>(this);
		Descriptor->SetDataObject(Message.Pawn);
		Descriptor->SetSceneComponent(Message.Pawn->FindComponentByClass<USkeletalMeshComponent>());
		Descriptor->SetProjectionMode(EActorCanvasProjectionMode::ComponentBoundingBox);
		Descriptor->SetIndicatorClass(NameplateWidgetClass);
		Descriptor->SetPriority(1);
		Descriptor->SetHAlign(HAlign_Center);
		Descriptor->SetVAlign(VAlign_Bottom);
		Descriptor->SetBoundingBoxAnchor(FVector(0.5f, 0.5f, 1.1f));
		Descriptor->SetIsHitTestable(true);

		IndicatorManager->AddIndicator(Descriptor);
		IndicatorOwnersMap.Emplace(Message.Pawn->GetActorLabel(), Descriptor);
	}
	else
	{
		IndicatorManager->RemoveIndicator(IndicatorOwnersMap.FindAndRemoveChecked(Message.Pawn->GetActorLabel()));
	}
}
