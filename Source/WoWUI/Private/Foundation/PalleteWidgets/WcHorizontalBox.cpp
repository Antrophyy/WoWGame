#include "Foundation/PalleteWidgets/WcHorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UWcHorizontalBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	for (int32 ChildIndex = 0; ChildIndex < GetChildrenCount(); ++ChildIndex)
	{
		if (ChildIndex == 0)
			continue;

		if (UHorizontalBoxSlot* VBoxSlot = Cast<UHorizontalBoxSlot>(GetChildAt(ChildIndex)->Slot))
		{
			VBoxSlot->SetPadding(FMargin(EntrySpacing, 0.f, 0.f, 0.f));
		}
	}
}
