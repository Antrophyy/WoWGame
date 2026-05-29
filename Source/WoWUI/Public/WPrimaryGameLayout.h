#pragma once

#include "EazyPrimaryGameLayout.h"
#include "WPrimaryGameLayout.generated.h"

class UEazyActivatableModalPriorityQueue;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWPrimaryGameLayout final : public UEazyPrimaryGameLayout
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
	TObjectPtr<UEazyActivatableModalPriorityQueue> Modal_Stack;
};