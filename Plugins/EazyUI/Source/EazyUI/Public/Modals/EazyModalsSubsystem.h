#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "EazyModalsSubsystem.generated.h"

class UEazyGameDialog;
class UEazyGameDialogDescriptor;

// Possible results from a dialog
UENUM(Category=UI)
enum class EEazyModalsResult : uint8
{
	Confirmed,
	Declined,
	Cancelled,
	Unknown UMETA(Hidden)
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FEazyModalsResultDelegate, EEazyModalsResult /* Result */, UEazyGameDialog* /*Dialog*/);

UCLASS(ClassGroup=UI)
class EAZYUI_API UEazyModalsSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	virtual void ShowConfirmation(UEazyGameDialogDescriptor* DialogDescriptor) {}
	virtual void ShowError(UEazyGameDialogDescriptor* DialogDescriptor) {}
};
