#include "Routing/RareActionBindingHandle.h"

#include "Routing/RareUIActionRouterBase.h"

uint32 GetTypeHash(const FRareActionBindingHandle& Handle)
{
	return HashCombine(::GetTypeHash(Handle.BindingId), PointerHash(Handle.Router.Get()));
}

bool FRareActionBindingHandle::IsValid() const
{
	return BindingId != INDEX_NONE;
}

void FRareActionBindingHandle::Unregister()
{
	if (URareUIActionRouterBase* RouterPtr = Router.Get())
	{
		RouterPtr->UnregisterInputAction(*this);
	}

	BindingId = INDEX_NONE;
	Router.Reset();
}



