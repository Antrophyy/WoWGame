#include "HUD/Modals/QuestLog/WcQuestLog.h"

#include "CommonListView.h"
#include "Components/TextBlock.h"
#include "HUD/Modals/QuestLog/WcQuestData.h"

void UWcQuestLog::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UWcQuestData*> Quests;
	Quests.Reserve(30);

	for (int i = 0; i < 30; ++i)
	{
		UWcQuestData* Quest = NewObject<UWcQuestData>();
		Quest->DisplayName = FText::Format(FText::FromString("Quest {0}"), i + 1);
		Quest->QuestDescription = FText::Format(FText::FromString("This is the description for quest {0}"), i + 1);

		Quests.Add(Quest);
	}

	Quests_ListView->SetListItems<UWcQuestData*>(Quests);
	Quests_ListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnQuestSelected);
}

void UWcQuestLog::OnQuestSelected(UObject* Quest)
{
	if (const UWcQuestData* QuestData = Cast<UWcQuestData>(Quest))
	{
		QuestDescription_Text->SetText(QuestData->QuestDescription);
	}
}
