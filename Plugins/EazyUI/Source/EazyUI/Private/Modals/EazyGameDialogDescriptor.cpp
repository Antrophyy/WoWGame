#include "Modals/EazyGameDialogDescriptor.h"

#include "LogEazyUI.h"

UEazyGameDialogDescriptor* UEazyGameDialogDescriptor::CreateConfirmation(const FText& Header, const FText& Body,
                                                                         const EEazyConfirmationDialogType Type,
                                                                         const FEazyModalsResultDelegate& ResultCallback,
                                                                         const bool bInShouldCloseAfterResolve,
                                                                         const int32 Priority)
{
	UEazyGameDialogDescriptor* Descriptor = CreateNewDescriptor(Header, Body, ResultCallback, bInShouldCloseAfterResolve, Priority);

	switch (Type)
	{
		case EEazyConfirmationDialogType::Ok:
			Descriptor->ButtonActions.Add(CreateOKAction());
			break;
		case EEazyConfirmationDialogType::OkCancel:
			Descriptor->ButtonActions.Add(CreateOKAction());
			Descriptor->ButtonActions.Add(CreateCancelAction());
			break;
		case EEazyConfirmationDialogType::Cancel:
			Descriptor->ButtonActions.Add(CreateCancelAction());
			break;
		case EEazyConfirmationDialogType::YesNo:
			Descriptor->ButtonActions.Add(CreateYesAction());
			Descriptor->ButtonActions.Add(CreateNoAction());
			break;
		case EEazyConfirmationDialogType::YesNoCancel:
			Descriptor->ButtonActions.Add(CreateYesAction());
			Descriptor->ButtonActions.Add(CreateNoAction());
			Descriptor->ButtonActions.Add(CreateCancelAction());
			break;
		default:
			UE_LOG(LogEazyUI, Error, TEXT("Invalid confirmation dialog type: %s."), *UEnum::GetValueAsString(Type));
			break;
	}

	return Descriptor;
}

UEazyGameDialogDescriptor* UEazyGameDialogDescriptor::CreateNewDescriptor(const FText& Header, const FText& Body, const FEazyModalsResultDelegate& ResultCallback, const bool bInShouldCloseAfterResolve, const int32 Priority)
{
	UEazyGameDialogDescriptor* Descriptor = NewObject<UEazyGameDialogDescriptor>();
	Descriptor->bShouldCloseAfterResolve = bInShouldCloseAfterResolve;
	Descriptor->Header = Header;
	Descriptor->Body = Body;
	Descriptor->ResultCallback = ResultCallback;
	Descriptor->Priority = Priority;

	return Descriptor;
}

FEazyConfirmationDialogAction UEazyGameDialogDescriptor::CreateOKAction()
{
	FEazyConfirmationDialogAction OKAction;
	OKAction.Result = EEazyModalsResult::Confirmed;
	OKAction.DisplayText = NSLOCTEXT("EazyUIModule", "ok", "OK");

	return OKAction;
}

FEazyConfirmationDialogAction UEazyGameDialogDescriptor::CreateYesAction()
{
	FEazyConfirmationDialogAction YesAction;
	YesAction.Result = EEazyModalsResult::Confirmed;
	YesAction.DisplayText = NSLOCTEXT("EazyUIModule", "yes", "Yes");

	return YesAction;
}

FEazyConfirmationDialogAction UEazyGameDialogDescriptor::CreateNoAction()
{
	FEazyConfirmationDialogAction NoAction;
	NoAction.Result = EEazyModalsResult::Declined;
	NoAction.DisplayText = NSLOCTEXT("EazyUIModule", "no", "No");

	return NoAction;
}

FEazyConfirmationDialogAction UEazyGameDialogDescriptor::CreateCancelAction()
{
	FEazyConfirmationDialogAction CancelAction;
	CancelAction.Result = EEazyModalsResult::Cancelled;
	CancelAction.DisplayText = NSLOCTEXT("EazyUIModule", "cancel", "Cancel");

	return CancelAction;
}
