#include "Game/HUD/Modals/QuestLog/WQuestLogEntryWidget.h"

#include "Components/TextBlock.h"
#include "Game/HUD/Modals/QuestLog/WcQuestData.h"

void UWQuestLogEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (const UWcQuestData* QuestData = Cast<UWcQuestData>(ListItemObject))
	{
		Name_Label->SetText(QuestData->DisplayName);
		Name_Label->SetText(QuestData->DisplayName);
	}
}
