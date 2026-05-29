#include "Routing/EazyActionBinding.h"

TMap<FEazyActionBindingHandle, TSharedPtr<FEazyActionBinding>> FEazyActionBinding::AllRegistrationsByHandle;

TSharedPtr<FEazyActionBinding> FEazyActionBinding::FindBinding(const FEazyActionBindingHandle& BindingHandle)
{
	if (TSharedPtr<FEazyActionBinding>* Ptr = AllRegistrationsByHandle.Find(BindingHandle))
	{
		return *Ptr;
	}

	return nullptr;
}

void FEazyActionBinding::AddBinding(FEazyActionBinding&& Binding)
{
	if (!Binding.Handle.IsValid())
	{
		return;
	}

	const FEazyActionBindingHandle BindingHandle = Binding.Handle;
	AllRegistrationsByHandle.Add(BindingHandle, MakeShared<FEazyActionBinding>(MoveTemp(Binding)));
}

void FEazyActionBinding::RemoveBinding(const FEazyActionBindingHandle& BindingHandle)
{
	AllRegistrationsByHandle.Remove(BindingHandle);
}

void FEazyActionBinding::ResetAllBindings()
{
	AllRegistrationsByHandle.Empty();
}
