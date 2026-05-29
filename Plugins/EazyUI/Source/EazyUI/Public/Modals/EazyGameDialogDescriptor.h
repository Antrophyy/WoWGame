#pragma once

#include "UObject/Object.h"
#include "EazyGameDialog.h"
#include "EazyModalsSubsystem.h"
#include "EazyGameDialogDescriptor.generated.h"

enum class EEazyConfirmationDialogType : uint8;

UCLASS(ClassGroup=UI)
class EAZYUI_API UEazyGameDialogDescriptor : public UObject
{
	GENERATED_BODY()

public:
	static UEazyGameDialogDescriptor* CreateConfirmation(const FText& Header,
	                                                     const FText& Body,
	                                                     EEazyConfirmationDialogType Type,
	                                                     const FEazyModalsResultDelegate& ResultCallback = FEazyModalsResultDelegate(),
	                                                     const bool bInShouldCloseAfterResolve = true,
	                                                     const int32 Priority = 0);

	FText Header;
	FText Body;

	TArray<FEazyConfirmationDialogAction> ButtonActions;
	bool bShouldCloseAfterResolve = true;

	FEazyModalsResultDelegate ResultCallback;

	int32 Priority = 0;

private:
	static UEazyGameDialogDescriptor* CreateNewDescriptor(const FText& Header,
	                                                      const FText& Body,
	                                                      const FEazyModalsResultDelegate& ResultCallback,
	                                                      const bool bInShouldCloseAfterResolve = true,
	                                                      const int32 Priority = 0);

	static FEazyConfirmationDialogAction CreateOKAction();
	static FEazyConfirmationDialogAction CreateYesAction();
	static FEazyConfirmationDialogAction CreateNoAction();
	static FEazyConfirmationDialogAction CreateCancelAction();
};
