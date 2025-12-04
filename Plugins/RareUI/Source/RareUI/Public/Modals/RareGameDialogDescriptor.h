#pragma once

#include "UObject/Object.h"
#include "RareGameDialog.h"
#include "RareModalsSubsystem.h"
#include "RareGameDialogDescriptor.generated.h"

enum class ERareConfirmationDialogType : uint8;

UCLASS(ClassGroup=UI)
class RAREUI_API URareGameDialogDescriptor : public UObject
{
	GENERATED_BODY()

public:
	static URareGameDialogDescriptor* CreateConfirmation(const FText& Header,
	                                                     const FText& Body,
	                                                     ERareConfirmationDialogType Type,
	                                                     const FRareModalsResultDelegate& ResultCallback = FRareModalsResultDelegate(),
	                                                     const bool bInShouldCloseAfterResolve = true);

	FText Header;
	FText Body;
	
	TArray<FRareConfirmationDialogAction> ButtonActions;
	ERareModalsPriority ErrorPriority = ERareModalsPriority::None;
	bool bShouldCloseAfterResolve = true;

	FRareModalsResultDelegate ResultCallback;

private:
	static URareGameDialogDescriptor* CreateNewDescriptor(const FText& Header,
	                                                      const FText& Body,
	                                                      const FRareModalsResultDelegate& ResultCallback,
	                                                      const bool bInShouldCloseAfterResolve = true);

	static FRareConfirmationDialogAction CreateOKAction();
	static FRareConfirmationDialogAction CreateYesAction();
	static FRareConfirmationDialogAction CreateNoAction();
	static FRareConfirmationDialogAction CreateCancelAction();
};
