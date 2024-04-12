#include "HUD/Modals/QuestLog/WcQuestLogEntry.h"

#include "Components/TextBlock.h"
#include "HUD/Modals/QuestLog/WcQuestData.h"

void UWcQuestLogEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (const UWcQuestData* QuestData = Cast<UWcQuestData>(ListItemObject))
	{
		Name_Label->SetText(QuestData->DisplayName);
		Name_Label->SetText(QuestData->DisplayName);
	}
}
