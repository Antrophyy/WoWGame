
#pragma once

#include "Widgets/CommonActivatableWidgetContainer.h"
#include "RareActivatableModalPriorityQueue.generated.h"

UCLASS()
class RAREUI_API URareActivatableModalPriorityQueue : public UCommonActivatableWidgetContainerBase
{
	GENERATED_BODY()

protected:
	virtual void OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget) override;
};
