#pragma once

#include "Foundation/WcActivatableWidget.h"
#include "WcHUDWidget.generated.h"

class UWcTargetingBehaviorComponent;
UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcHUDWidget final : public UWcActivatableWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> PauseScreenClass;
	
private: // Actions

	void HandleEscapeAction();

	FUIActionBindingHandle EscapeActionHandle;

private: // Internal Properties

	TWeakObjectPtr<UWcTargetingBehaviorComponent> TargetingBehaviorComponent;
};
