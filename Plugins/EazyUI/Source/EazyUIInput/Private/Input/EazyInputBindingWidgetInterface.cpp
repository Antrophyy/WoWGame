#include "Input/EazyInputBindingWidgetInterface.h"

#include "Blueprint/UserWidget.h"
#include "Routing/EazyUIActionRouterBase.h"

FEazyActionBindingHandle IEazyInputBindingWidgetInterface::RegisterInputActionBindingForWidget(UUserWidget& Widget, const FEazyInputActionBindingArgs& BindActionArgs, TArray<FEazyActionBindingHandle>& InOutBoundInputActions) const
{
	if (UEazyUIActionRouterBase* Router = UEazyUIActionRouterBase::Get(Widget))
	{
		const FEazyActionBindingHandle Handle = Router->RegisterInputAction(Widget, BindActionArgs);
		if (Handle.IsValid())
		{
			InOutBoundInputActions.Add(Handle);
		}

		return Handle;
	}

	return FEazyActionBindingHandle();
}

void IEazyInputBindingWidgetInterface::RemoveInputActionBindingForWidget(const FEazyActionBindingHandle& ActionBinding, TArray<FEazyActionBindingHandle>& InOutBoundInputActions) const
{
	const int32 BindingIndex = InOutBoundInputActions.IndexOfByKey(ActionBinding);
	if (BindingIndex == INDEX_NONE)
	{
		return;
	}

	InOutBoundInputActions[BindingIndex].Unregister();
	InOutBoundInputActions.RemoveAtSwap(BindingIndex);
}

void IEazyInputBindingWidgetInterface::RemoveAllInputActionBindingsForWidget(TArray<FEazyActionBindingHandle>& InOutBoundInputActions) const
{
	for (FEazyActionBindingHandle& Handle : InOutBoundInputActions)
	{
		Handle.Unregister();
	}

	InOutBoundInputActions.Empty();
}

