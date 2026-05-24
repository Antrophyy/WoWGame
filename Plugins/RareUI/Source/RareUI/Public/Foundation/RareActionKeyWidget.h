// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "RareUserWidget.h"
#include "RareActionKeyWidget.generated.h"

class URareEnhancedActionWidget;
class UInputAction;

UCLASS(Abstract, ClassGroup=UI, Category=UI)
class RAREUI_API URareActionKeyWidget : public URareUserWidget
{
	GENERATED_BODY()

public:
	URareEnhancedActionWidget* GetActionWidget();

protected: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URareEnhancedActionWidget> ActionWidget;
};
