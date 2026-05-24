// Copyright (C) Grip Studios. All Rights Reserved

#include "Modals/RareActivatableModalPriorityQueue.h"

#include "Modals/RareGameDialog.h"

void URareActivatableModalPriorityQueue::OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget)
{
	// When a new modal is added, sort the list by priority (higher priority first)
	WidgetList.Sort([](const UCommonActivatableWidget& A, const UCommonActivatableWidget& B)
	{
		const URareGameDialog* DialogA = Cast<URareGameDialog>(&A);
		const URareGameDialog* DialogB = Cast<URareGameDialog>(&B);

		const int32 PriorityA = IsValid(DialogA) ? DialogA->Priority : 0;
		const int32 PriorityB = IsValid(DialogB) ? DialogB->Priority : 0;

		// Higher priority comes to the top.
		return PriorityA < PriorityB;
	});

	if (WidgetList.IsEmpty())
	{
		return;
	}

	if (GetActiveWidget() == WidgetList[0])
	{
		// We're already showing the highest priority modal. Just need to reorder the rest of the widgets in the switcher
		for (int32 i = 1; i < WidgetList.Num(); ++i)
		{
			MySwitcher->RemoveSlot(WidgetList[i]->TakeWidget());
		}
		
		// Re-add the widgets in order
		for (int32 i = 1; i < WidgetList.Num(); ++i)
		{
			MySwitcher->AddSlot() [WidgetList[i]->TakeWidget()];
		}
	}
	else
	{
		for (int32 i = 0; i < WidgetList.Num(); ++i)
		{
			// Remove all widgets from the switcher
			MySwitcher->RemoveSlot(WidgetList[i]->TakeWidget());
		}
		
		for (int32 i = 0; i < WidgetList.Num(); ++i)
		{
			// Re-add the widgets in order
			MySwitcher->AddSlot() [WidgetList[i]->TakeWidget()];
		}
	}
	
	SetSwitcherIndex(MySwitcher->GetNumWidgets() - 1);
}
