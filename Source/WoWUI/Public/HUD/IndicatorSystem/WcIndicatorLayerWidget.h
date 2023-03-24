#pragma once

#include "Foundation/WcUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "WcIndicatorLayerWidget.generated.h"

class UWcIndicatorComponent_Nameplates;
class UBaseIndicatorWidget;

UCLASS(Abstract)
class WOWUI_API UWcIndicatorLayerWidget final : public UWcUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UBaseIndicatorWidget> NameplateWidgetClass;

private:

	void CreateComponents();

private: // Components

	UPROPERTY()
	TObjectPtr<UWcIndicatorComponent_Nameplates> NameplatesComponent;
};
