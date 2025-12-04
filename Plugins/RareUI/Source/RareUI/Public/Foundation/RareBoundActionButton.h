#pragma once

#include "Input/CommonBoundActionButton.h"
#include "RareBoundActionButton.generated.h"

class URareEnhancedActionWidget;
// We don't want to allow the creation of this class, so we make it abstract. Create project specific ones instead.
UCLASS(Abstract, ClassGroup=UI)
class RAREUI_API URareBoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()

public:
	void SetDisplayText(const FText& InDisplayText);

	virtual void SetRepresentedAction(FUIActionBindingHandle InBindingHandle) override;

protected:

	/** Begin UUserWidget */
	virtual void NativeConstruct() override;
	/** End UUserWidget */

	/** Begin UCommonActionWidget */
	virtual void UpdateInputActionWidget() override;
	/** End UCommonActionWidget */
	
	UPROPERTY()
	TObjectPtr<URareEnhancedActionWidget> RareEnhancedActionWidget;
	
private: // Internal

	bool bCustomDisplayText = false;

	FText CustomDisplayText;
};
