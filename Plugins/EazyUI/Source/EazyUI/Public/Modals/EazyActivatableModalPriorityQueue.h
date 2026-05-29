#pragma once

#include "Widgets/CommonActivatableWidgetContainer.h"
#include "EazyActivatableModalPriorityQueue.generated.h"

UCLASS()
class EAZYUI_API UEazyActivatableModalPriorityQueue : public UCommonActivatableWidgetContainerBase
{
	GENERATED_BODY()

protected:
	virtual void OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget) override;
};
