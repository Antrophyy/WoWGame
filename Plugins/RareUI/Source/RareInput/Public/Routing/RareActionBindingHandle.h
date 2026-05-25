#pragma once

#include "EnhancedInputComponent.h"

struct FRareActionBindingHandle;
RAREINPUT_API uint32 GetTypeHash(const FRareActionBindingHandle& Handle);

/**
 * A handle representing a registered Rare UI Enhanced Input action binding.
 * Returned by URareUIActionRouterBase::RegisterInputAction.
 */
struct RAREINPUT_API FRareActionBindingHandle
{
	FRareActionBindingHandle() = default;

	/** Returns true if this handle refers to a valid (registered) binding. */
	bool IsValid() const;

	/** Unregisters this binding through the owning router and then clears the handle. */
	void Unregister();

	bool operator==(const FRareActionBindingHandle& Other) const { return BindingId == Other.BindingId && Router == Other.Router; }
	bool operator!=(const FRareActionBindingHandle& Other) const { return !(*this == Other); }

private:
	friend class URareUIActionRouterBase;
	friend struct FRareActionBinding;
	friend RAREINPUT_API uint32 GetTypeHash(const FRareActionBindingHandle& Handle);
	explicit FRareActionBindingHandle(const int32 InId) : BindingId(InId) {}

	int32 BindingId = INDEX_NONE;
	TWeakObjectPtr<class URareUIActionRouterBase> Router;
};



