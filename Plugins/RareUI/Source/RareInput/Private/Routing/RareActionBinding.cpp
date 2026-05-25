#include "Routing/RareActionBinding.h"

TMap<FRareActionBindingHandle, TSharedPtr<FRareActionBinding>> FRareActionBinding::AllRegistrationsByHandle;

TSharedPtr<FRareActionBinding> FRareActionBinding::FindBinding(const FRareActionBindingHandle& BindingHandle)
{
	if (TSharedPtr<FRareActionBinding>* Ptr = AllRegistrationsByHandle.Find(BindingHandle))
	{
		return *Ptr;
	}

	return nullptr;
}

void FRareActionBinding::AddBinding(FRareActionBinding&& Binding)
{
	if (!Binding.Handle.IsValid())
	{
		return;
	}

	const FRareActionBindingHandle BindingHandle = Binding.Handle;
	AllRegistrationsByHandle.Add(BindingHandle, MakeShared<FRareActionBinding>(MoveTemp(Binding)));
}

void FRareActionBinding::RemoveBinding(const FRareActionBindingHandle& BindingHandle)
{
	AllRegistrationsByHandle.Remove(BindingHandle);
}

void FRareActionBinding::ResetAllBindings()
{
	AllRegistrationsByHandle.Empty();
}
