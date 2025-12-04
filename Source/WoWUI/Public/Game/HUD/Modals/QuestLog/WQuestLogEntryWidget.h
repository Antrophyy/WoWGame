#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Foundation/WUserWidget.h"
#include "WQuestLogEntryWidget.generated.h"

class UTextBlock;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWQuestLogEntryWidget : public UWUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Name_Label;
};
