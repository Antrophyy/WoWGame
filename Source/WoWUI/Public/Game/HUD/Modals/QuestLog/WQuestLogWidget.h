#pragma once

#include "Foundation/WDraggableModalWidget.h"
#include "Foundation/WUserWidget.h"
#include "WQuestLogWidget.generated.h"

class UTextBlock;
class UCommonListView;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWQuestLogWidget : public UWDraggableModalWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private: // Internal
	
	void HandleQuestSelected(UObject* Quest);

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonListView> Quests_ListView;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> QuestDescription_Text;
};
