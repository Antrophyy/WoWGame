#include "Foundation/RareListView/IUserStructListEntry.h"

UUserStructListEntry::UUserStructListEntry(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
}

void IUserStructListEntry::NativeOnListItemSet(TSharedPtr<FRareListRowBase> ListItem)
{
	
}

void IUserStructListEntry::SetListItem(UUserWidget& ListEntryWidget, TSharedPtr<FRareListRowBase> ListItem)
{
	if (IUserStructListEntry* NativeImplementation = Cast<IUserStructListEntry>(&ListEntryWidget))
	{
		NativeImplementation->NativeOnListItemSet(ListItem);
	}
}

TSharedPtr<FRareListRowBase> UUserStructListEntryLibrary::GetListItem(TScriptInterface<IUserStructListEntry> UserStructListEntry)
{
	if (UUserWidget* EntryWidget = Cast<UUserWidget>(UserStructListEntry.GetObject()))
	{
		const URareListView* OwningListView = Cast<URareListView>(UUserListEntryLibrary::GetOwningListView(EntryWidget));
		if (const TSharedPtr<FRareListRowBase>* ListItem = OwningListView ? OwningListView->ItemFromEntryWidget(*EntryWidget) : nullptr)
		{
			return *ListItem;
		}
	}
	return nullptr;
}

