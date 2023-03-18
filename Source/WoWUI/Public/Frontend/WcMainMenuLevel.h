#pragma once

#include "Engine/LevelScriptActor.h"
#include "WcMainMenuLevel.generated.h"

class UCommonActivatableWidget;

UCLASS()
class WOWUI_API AWcMainMenuLevel final : public ALevelScriptActor
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

protected: // Set from the Editor

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> MainMenuScreenClass;
};
