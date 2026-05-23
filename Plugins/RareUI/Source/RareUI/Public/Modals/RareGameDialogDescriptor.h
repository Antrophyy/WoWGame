// Copyright (C) Grip Studios. All Rights Reserved

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
	                                                     const bool bInShouldCloseAfterResolve = true,
	                                                     const int32 Priority = 0);

	FText Header;
	FText Body;

	TArray<FRareConfirmationDialogAction> ButtonActions;
	bool bShouldCloseAfterResolve = true;

	FRareModalsResultDelegate ResultCallback;

	int32 Priority = 0;

private:
	static URareGameDialogDescriptor* CreateNewDescriptor(const FText& Header,
	                                                      const FText& Body,
	                                                      const FRareModalsResultDelegate& ResultCallback,
	                                                      const bool bInShouldCloseAfterResolve = true,
	                                                      const int32 Priority = 0);

	static FRareConfirmationDialogAction CreateOKAction();
	static FRareConfirmationDialogAction CreateYesAction();
	static FRareConfirmationDialogAction CreateNoAction();
	static FRareConfirmationDialogAction CreateCancelAction();
};
