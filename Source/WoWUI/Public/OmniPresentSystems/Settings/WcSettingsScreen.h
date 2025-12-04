#pragma once

#include "Foundation/WActivatableWidget.h"
#include "WcSettingsScreen.generated.h"

UCLASS(ClassGroup=UI, Abstract)
class WOWUI_API UWcSettingsScreen : public UWActivatableWidget
{
	GENERATED_BODY()

public:

	void DoStuff();
};
