#pragma once

#include "InputAction.h"
#include "Routing/RareActionBindingHandle.h"

class UCommonActivatableWidget;
class UWidget;

/**
 * RareUI equivalent of FUIActionBinding.
 * Stores binding metadata and provides global lookup by FRareActionBindingHandle.
 */
struct RAREINPUT_API FRareActionBinding
{
	int32 BindingId = INDEX_NONE;
	FRareActionBindingHandle Handle;
	TWeakObjectPtr<const UWidget> RegisteringWidget;
	TWeakObjectPtr<const UCommonActivatableWidget> OwningActivatable;
	TWeakObjectPtr<UInputAction> InputAction;
	ETriggerEvent TriggerEvent = ETriggerEvent::None;
	FSimpleDelegate Callback;
	
	FText DisplayName;
	int32 ActionBarPriority = 0;
	
	bool bDisplayInActionBar = true;

	static TSharedPtr<FRareActionBinding> FindBinding(const FRareActionBindingHandle& BindingHandle);
	static void AddBinding(FRareActionBinding&& Binding);
	static void RemoveBinding(const FRareActionBindingHandle& BindingHandle);
	static void ResetAllBindings();

private:
	static TMap<FRareActionBindingHandle, TSharedPtr<FRareActionBinding>> AllRegistrationsByHandle;
};


