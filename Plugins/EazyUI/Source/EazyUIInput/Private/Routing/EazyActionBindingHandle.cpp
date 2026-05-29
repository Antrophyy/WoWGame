#include "Routing/EazyActionBindingHandle.h"

#include "Routing/EazyUIActionRouterBase.h"

uint32 GetTypeHash(const FEazyActionBindingHandle& Handle)
{
	return HashCombine(::GetTypeHash(Handle.BindingId), PointerHash(Handle.Router.Get()));
}

bool FEazyActionBindingHandle::IsValid() const
{
	return BindingId != INDEX_NONE;
}

void FEazyActionBindingHandle::Unregister()
{
	if (UEazyUIActionRouterBase* RouterPtr = Router.Get())
	{
		RouterPtr->UnregisterInputAction(*this);
	}

	BindingId = INDEX_NONE;
	Router.Reset();
}
