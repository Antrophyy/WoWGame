#pragma once

#include "CommonButtonBase.h"
#include "EazyButtonBase.generated.h"

// We don't want to allow the creation of this class, so we make it abstract. Create project specific ones instead.
UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void ForceClick();

	void ForceHover();
	void ForceUnhover();

	virtual void NativeConstruct() override;

protected: // Overridable Functions

	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="Reset Animations State"))
	void BP_ResetAnimationsState();
	
	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	virtual void NativeResetAnimationsState();
};
