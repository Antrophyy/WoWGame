#pragma once

#include "UObject/Object.h"
#include "WcQuestData.generated.h"

UCLASS(ClassGroup=UI)
class WOWUI_API UWcQuestData : public UObject
{
	GENERATED_BODY()

public:
	FText DisplayName;
	FText QuestDescription;
};
