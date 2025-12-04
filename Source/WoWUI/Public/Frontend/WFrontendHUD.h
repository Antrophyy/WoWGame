#pragma once

#include "RareHUD.h"
#include "WFrontendHUD.generated.h"

class UCommonActivatableWidget;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API AWFrontendHUD : public ARareHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
protected: // Set by the Editor
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> MainMenuScreen_Class;
};
