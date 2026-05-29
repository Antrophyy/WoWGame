#pragma once

#include "EazyUserWidget.h"
#include "EazyActionKeyWidget.generated.h"

class UEazyEnhancedActionWidget;
class UInputAction;

UCLASS(Abstract, ClassGroup=UI, Category=UI)
class EAZYUI_API UEazyActionKeyWidget : public UEazyUserWidget
{
	GENERATED_BODY()

public:
	UEazyEnhancedActionWidget* GetActionWidget();

protected: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEazyEnhancedActionWidget> ActionWidget;
};
