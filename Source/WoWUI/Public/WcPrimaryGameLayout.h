#pragma once

#include "PrimaryGameLayout.h"
#include "WcPrimaryGameLayout.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcPrimaryGameLayout final : public UPrimaryGameLayout
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MenuScreenStack;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> ModalStack;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> GameStack;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> ErrorStack;
};
