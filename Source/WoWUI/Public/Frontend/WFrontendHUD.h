#pragma once

#include "EazyHUD.h"
#include "WFrontendHUD.generated.h"

class UEazyCommonActivatableWidget;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API AWFrontendHUD : public AEazyHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
protected: // Set by the Editor
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UEazyCommonActivatableWidget> MainMenuScreen_Class;
};
