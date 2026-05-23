// Copyright (C) Grip Studios. All Rights Reserved

#include "Modals/RareGameDialogDescriptor.h"

#include "LogRareUI.h"

URareGameDialogDescriptor* URareGameDialogDescriptor::CreateConfirmation(const FText& Header, const FText& Body,
                                                                         const ERareConfirmationDialogType Type,
                                                                         const FRareModalsResultDelegate& ResultCallback,
                                                                         const bool bInShouldCloseAfterResolve,
                                                                         const int32 Priority)
{
	URareGameDialogDescriptor* Descriptor = CreateNewDescriptor(Header, Body, ResultCallback, bInShouldCloseAfterResolve, Priority);

	switch (Type)
	{
		case ERareConfirmationDialogType::Ok:
			Descriptor->ButtonActions.Add(CreateOKAction());
			break;
		case ERareConfirmationDialogType::OkCancel:
			Descriptor->ButtonActions.Add(CreateOKAction());
			Descriptor->ButtonActions.Add(CreateCancelAction());
			break;
		case ERareConfirmationDialogType::Cancel:
			Descriptor->ButtonActions.Add(CreateCancelAction());
			break;
		case ERareConfirmationDialogType::YesNo:
			Descriptor->ButtonActions.Add(CreateYesAction());
			Descriptor->ButtonActions.Add(CreateNoAction());
			break;
		case ERareConfirmationDialogType::YesNoCancel:
			Descriptor->ButtonActions.Add(CreateYesAction());
			Descriptor->ButtonActions.Add(CreateNoAction());
			Descriptor->ButtonActions.Add(CreateCancelAction());
			break;
		default:
			UE_LOG(LogRareUI, Error, TEXT("Invalid confirmation dialog type: %s."), *UEnum::GetValueAsString(Type));
			break;
	}

	return Descriptor;
}

URareGameDialogDescriptor* URareGameDialogDescriptor::CreateNewDescriptor(const FText& Header, const FText& Body, const FRareModalsResultDelegate& ResultCallback, const bool bInShouldCloseAfterResolve, const int32 Priority)
{
	URareGameDialogDescriptor* Descriptor = NewObject<URareGameDialogDescriptor>();
	Descriptor->bShouldCloseAfterResolve = bInShouldCloseAfterResolve;
	Descriptor->Header = Header;
	Descriptor->Body = Body;
	Descriptor->ResultCallback = ResultCallback;
	Descriptor->Priority = Priority;

	return Descriptor;
}

FRareConfirmationDialogAction URareGameDialogDescriptor::CreateOKAction()
{
	FRareConfirmationDialogAction OKAction;
	OKAction.Result = ERareModalsResult::Confirmed;
	OKAction.DisplayText = NSLOCTEXT("RareUIModule", "ok", "OK");

	return OKAction;
}

FRareConfirmationDialogAction URareGameDialogDescriptor::CreateYesAction()
{
	FRareConfirmationDialogAction YesAction;
	YesAction.Result = ERareModalsResult::Confirmed;
	YesAction.DisplayText = NSLOCTEXT("RareUIModule", "yes", "Yes");

	return YesAction;
}

FRareConfirmationDialogAction URareGameDialogDescriptor::CreateNoAction()
{
	FRareConfirmationDialogAction NoAction;
	NoAction.Result = ERareModalsResult::Declined;
	NoAction.DisplayText = NSLOCTEXT("RareUIModule", "no", "No");

	return NoAction;
}

FRareConfirmationDialogAction URareGameDialogDescriptor::CreateCancelAction()
{
	FRareConfirmationDialogAction CancelAction;
	CancelAction.Result = ERareModalsResult::Cancelled;
	CancelAction.DisplayText = NSLOCTEXT("RareUIModule", "cancel", "Cancel");

	return CancelAction;
}
