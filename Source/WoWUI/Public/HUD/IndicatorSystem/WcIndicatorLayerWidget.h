#pragma once

#include "Foundation/WcUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "WcIndicatorLayerWidget.generated.h"

class UBaseIndicatorWidget;
class UIndicatorDescriptor;
struct FEnemySpawnedMessage;

UCLASS(Abstract)
class WOWUI_API UWcIndicatorLayerWidget final : public UWcUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UBaseIndicatorWidget> NameplateWidgetClass;

private:

	void RegisterListeners();

	void UnregisterListeners();

private: // Broadcast Handling

	void HandleEnemyAggroStateChanged(const FGameplayTag GameplayTag, const FEnemySpawnedMessage& Message);

	FGameplayMessageListenerHandle EnemySpawnedListener;

private: // Internal Properties

	UPROPERTY()
	TMap<FString, TObjectPtr<UIndicatorDescriptor>> IndicatorOwnersMap; 
};
