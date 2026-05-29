#pragma once

#include "Input/CommonBoundActionButton.h"
#include "EazyBoundActionButton.generated.h"

struct FEazyActionBindingHandle;
class UEazyEnhancedActionWidget;

// We don't want to allow the creation of this class, so we make it abstract. Create project specific ones instead.
UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyBoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()

public:
	void SetDisplayText(const FText& InDisplayText);

	void SetRepresentedInputAction(FEazyActionBindingHandle InBindingHandle) const;

protected:

	// Begin UUserWidget
	virtual void NativeConstruct() override;
	// End UUserWidget

	// Begin UCommonActionWidget
	virtual void UpdateInputActionWidget() override;
	// End UCommonActionWidget
	
	UPROPERTY()
	TObjectPtr<UEazyEnhancedActionWidget> EazyEnhancedActionWidget;
	
private: // Internal

	bool bCustomDisplayText = false;

	FText CustomDisplayText;
};
