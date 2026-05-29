#pragma once

#include "EnhancedInputComponent.h"

struct FEazyActionBindingHandle;
class UEazyUIActionRouterBase;

EAZYUIINPUT_API uint32 GetTypeHash(const FEazyActionBindingHandle& Handle);

// A handle representing a registered EazyUI Enhanced Input action binding.
// Returned by UEazyUIActionRouterBase::RegisterInputAction.
struct EAZYUIINPUT_API FEazyActionBindingHandle
{
	FEazyActionBindingHandle() = default;

	// Returns true if this handle refers to a valid (registered) binding.
	bool IsValid() const;

	// Unregisters this binding through the owning router and then clears the handle.
	void Unregister();

	bool operator==(const FEazyActionBindingHandle& Other) const { return BindingId == Other.BindingId && Router == Other.Router; }
	bool operator!=(const FEazyActionBindingHandle& Other) const { return !(*this == Other); }

private:
	friend class UEazyUIActionRouterBase;
	friend struct FEazyActionBinding;
	friend EAZYUIINPUT_API uint32 GetTypeHash(const FEazyActionBindingHandle& Handle);
	explicit FEazyActionBindingHandle(const int32 InId) : BindingId(InId) {}

	int32 BindingId = INDEX_NONE;
	TWeakObjectPtr<UEazyUIActionRouterBase> Router;
};
