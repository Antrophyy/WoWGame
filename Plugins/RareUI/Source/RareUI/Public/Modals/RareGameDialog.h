#pragma once

#include "Foundation/RareCommonActivatableWidget.h"
#include "RareGameDialog.generated.h"

enum class ERareModalsResult : uint8;
class URareGameDialogDescriptor;

UENUM(Category=UI)
enum class ERareConfirmationDialogType : uint8
{
	Ok,
	OkCancel,
	Cancel,
	YesNo,
	YesNoCancel
};


USTRUCT(Category=UI)
struct FRareConfirmationDialogAction
{
	GENERATED_BODY()

	ERareModalsResult Result;
	FText DisplayText;
};

UCLASS(Abstract, ClassGroup=UI)
class RAREUI_API URareGameDialog : public URareCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void SetupDialog(URareGameDialogDescriptor* Descriptor);
	URareGameDialogDescriptor* GetDialogDescriptor() const { return DialogDescriptor; }

private:
	UPROPERTY()
	TObjectPtr<URareGameDialogDescriptor> DialogDescriptor;
};
