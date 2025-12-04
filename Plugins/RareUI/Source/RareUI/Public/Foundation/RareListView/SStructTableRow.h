#pragma once

#include "Slate/SObjectTableRow.h"
#include "IUserStructListEntry.h"
#include "RareListView.h"

template <typename ItemType>
class RAREUI_API SStructTableRow : public SObjectTableRow<ItemType>
{
};

template <>
inline void SObjectTableRow<TSharedPtr<FRareListRowBase>>::InitObjectRowInternal(UUserWidget& ListEntryWidget, TSharedPtr<FRareListRowBase> ListItemObject)
{
	if (ListEntryWidget.Implements<UUserStructListEntry>())
	{
		IUserStructListEntry::SetListItem(*WidgetObject, ListItemObject);
	}
}