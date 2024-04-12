#pragma once

#include "Foundation/WcActivatableWidget.h"
#include "WcSettingsScreen.generated.h"

UCLASS(ClassGroup=UI, Abstract)
class WOWUI_API UWcSettingsScreen : public UWcActivatableWidget
{
	GENERATED_BODY()

public:

	void DoStuff();
};
