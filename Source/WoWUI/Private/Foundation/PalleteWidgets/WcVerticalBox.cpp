#include "Foundation/PalleteWidgets/WcVerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UWcVerticalBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	for (int32 ChildIndex = 0; ChildIndex < GetChildrenCount(); ++ChildIndex)
	{
		if (ChildIndex == 0)
			continue;

		if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(GetChildAt(ChildIndex)->Slot))
		{
			VBoxSlot->SetPadding(FMargin(0.f, EntrySpacing, 0.f, 0.f));
		}
	}
}
