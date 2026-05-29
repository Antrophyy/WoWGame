#pragma once

#include "Foundation/EazyCommonActivatableWidget.h"
#include "EazyGameDialog.generated.h"

enum class EEazyModalsResult : uint8;
class UEazyGameDialogDescriptor;

UENUM(Category=UI)
enum class EEazyConfirmationDialogType : uint8
{
	Ok,
	OkCancel,
	Cancel,
	YesNo,
	YesNoCancel
};

USTRUCT(Category=UI)
struct FEazyConfirmationDialogAction
{
	GENERATED_BODY()

	EEazyModalsResult Result;
	FText DisplayText;
};

UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyGameDialog : public UEazyCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void SetupDialog(UEazyGameDialogDescriptor* Descriptor);
	UEazyGameDialogDescriptor* GetDialogDescriptor() const { return DialogDescriptor; }
	
	// Priority of this dialog in the modal queue. Higher priority dialogs are shown first.
	int32 Priority = 0;

private:
	UPROPERTY()
	TObjectPtr<UEazyGameDialogDescriptor> DialogDescriptor;
};
