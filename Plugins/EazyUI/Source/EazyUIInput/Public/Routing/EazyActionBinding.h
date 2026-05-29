#pragma once

#include "InputAction.h"
#include "Routing/EazyActionBindingHandle.h"

class UCommonActivatableWidget;
class UWidget;

// EazyUI equivalent of FUIActionBinding.
// Stores binding metadata and provides global lookup by FEazyActionBindingHandle.
struct EAZYUIINPUT_API FEazyActionBinding
{
	int32 BindingId = INDEX_NONE;
	FEazyActionBindingHandle Handle;
	TWeakObjectPtr<const UWidget> RegisteringWidget;
	TWeakObjectPtr<const UCommonActivatableWidget> OwningActivatable;
	TWeakObjectPtr<UInputAction> InputAction;
	ETriggerEvent TriggerEvent = ETriggerEvent::None;
	FSimpleDelegate Callback;
	
	FText DisplayName;
	int32 ActionBarPriority = 0;
	
	bool bDisplayInActionBar = true;

	static TSharedPtr<FEazyActionBinding> FindBinding(const FEazyActionBindingHandle& BindingHandle);
	static void AddBinding(FEazyActionBinding&& Binding);
	static void RemoveBinding(const FEazyActionBindingHandle& BindingHandle);
	static void ResetAllBindings();

private:
	static TMap<FEazyActionBindingHandle, TSharedPtr<FEazyActionBinding>> AllRegistrationsByHandle;
};
