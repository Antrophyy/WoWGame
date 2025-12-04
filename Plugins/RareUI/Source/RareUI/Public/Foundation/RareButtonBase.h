#pragma once

#include "CommonButtonBase.h"
#include "RareButtonBase.generated.h"

// We don't want to allow the creation of this class, so we make it abstract. Create project specific ones instead.
UCLASS(Abstract, ClassGroup=UI)
class RAREUI_API URareButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void ForceClick();

	void ForceHover();
	void ForceUnhover();

protected:
	virtual void NativeConstruct() override;

protected: // Overridable Functions

	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="Reset Animations State"))
	void BP_ResetAnimationsState();
	
	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	virtual void NativeResetAnimationsState();
};
