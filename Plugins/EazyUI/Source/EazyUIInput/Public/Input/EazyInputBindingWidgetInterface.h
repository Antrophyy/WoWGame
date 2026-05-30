#pragma once

#include "UObject/Interface.h"
#include "EazyInputBindingWidgetInterface.generated.h"

struct FEazyActionBindingHandle;
struct FEazyInputActionBindingArgs;
class UUserWidget;

UINTERFACE(MinimalAPI)
class UEazyInputBindingWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class EAZYUIINPUT_API IEazyInputBindingWidgetInterface
{
	GENERATED_BODY()

protected:
	FEazyActionBindingHandle RegisterInputActionBindingForWidget(UUserWidget& Widget, const FEazyInputActionBindingArgs& BindActionArgs, TArray<FEazyActionBindingHandle>& InOutBoundInputActions) const;
	void RemoveInputActionBindingForWidget(const FEazyActionBindingHandle& ActionBinding, TArray<FEazyActionBindingHandle>& InOutBoundInputActions) const;
	void RemoveAllInputActionBindingsForWidget(TArray<FEazyActionBindingHandle>& InOutBoundInputActions) const;
};

