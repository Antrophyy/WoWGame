#pragma once

#include "Widgets/CommonActivatableWidgetContainer.h"
#include "RareCommonActivatableWidgetStack.generated.h"

UCLASS(ClassGroup=UI)
class RAREUI_API URareCommonActivatableWidgetStack : public UCommonActivatableWidgetStack
{
	GENERATED_BODY()

public:
	FUserWidgetPool& GetGeneratedWidgetsPool() { return GeneratedWidgetsPool; }
};
