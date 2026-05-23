#pragma once

#include "RarePrimaryGameLayout.h"
#include "WPrimaryGameLayout.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWPrimaryGameLayout final : public URarePrimaryGameLayout
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Game_Stack;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> GameWindow_Stack;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MenuScreen_Stack;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Modal_Stack;
};