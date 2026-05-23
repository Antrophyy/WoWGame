// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "RareModalsSubsystem.generated.h"

class URareGameDialog;
class URareGameDialogDescriptor;

// Possible results from a dialog
UENUM(Category=UI)
enum class ERareModalsResult : uint8
{
	Confirmed,
	Declined,
	Cancelled,
	Unknown UMETA(Hidden)
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FRareModalsResultDelegate, ERareModalsResult /* Result */, URareGameDialog* /*Dialog*/);

UCLASS(ClassGroup=UI)
class RAREUI_API URareModalsSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	virtual void ShowConfirmation(URareGameDialogDescriptor* DialogDescriptor) {}
	virtual void ShowError(URareGameDialogDescriptor* DialogDescriptor) {}
};
