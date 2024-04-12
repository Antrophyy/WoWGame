#pragma once

#include "Foundation/WcDraggableModalWidget.h"
#include "Foundation/WcUserWidget.h"
#include "WcQuestLog.generated.h"

class UTextBlock;
class UCommonListView;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcQuestLog : public UWcDraggableModalWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;


private:
	void OnQuestSelected(UObject* Quest);

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonListView> Quests_ListView;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> QuestDescription_Text;
};
