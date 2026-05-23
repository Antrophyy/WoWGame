// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareListView/RareListView.h"

#include "Foundation/RareListView/SStructTableRow.h"

URareListView::URareListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URareListView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyListView.Reset();
}

#if WITH_EDITOR
void URareListView::OnRefreshDesignerItems()
{
	RefreshDesignerItems<TSharedPtr<FRareListRowBase>>(ListItems, [this] () {return MakeShared<FStructDesignerPreview>(); });
}

#endif

void URareListView::PostLoad()
{
	Super::PostLoad();
}

void URareListView::SetSelectedItem(const FRareListRowBase& Item)
{
	SetSelectedItemInternal(MakeShared<FRareListRowBase>(Item));
}

void URareListView::SetSelectedItemInternal(TSharedPtr<FRareListRowBase> Item)
{
	ITypedUMGListView<TSharedPtr<FRareListRowBase>>::SetSelectedItem(Item);
}

void URareListView::AddItem(const FRareListRowBase& Item)
{
	const TSharedPtr<FRareListRowBase> ItemPtr = MakeShared<FRareListRowBase>(Item);
	
	if (ItemPtr == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Cannot add null item into ListView."), ELogVerbosity::Warning, "NullListViewItem");
		return;
	}

	if (ListItems.Contains(ItemPtr))
	{
		FFrame::KismetExecutionMessage(TEXT("Cannot add duplicate item into ListView."), ELogVerbosity::Warning, "DuplicateListViewItem");
		return;
	}

	ListItems.Add(ItemPtr);

	const TArray<TSharedPtr<FRareListRowBase>> Added = { ItemPtr };
	const TArray<TSharedPtr<FRareListRowBase>> Removed;

	RequestRefresh();
}

void URareListView::RemoveItem(const FRareListRowBase& Item)
{
	const TSharedPtr<FRareListRowBase> ItemPtr = MakeShared<FRareListRowBase>(Item);

	ListItems.Remove(ItemPtr);

	const TArray<TSharedPtr<FRareListRowBase>> Added;
	const TArray<TSharedPtr<FRareListRowBase>> Removed = { ItemPtr };

	RequestRefresh();
}

TSharedPtr<FRareListRowBase> URareListView::GetItemAt(int32 Index) const
{
	return ListItems.IsValidIndex(Index) ? ListItems[Index] : nullptr;
}

int32 URareListView::GetNumItems() const
{
	return ListItems.Num();
}

int32 URareListView::GetIndexForItem(const FRareListRowBase& Item) const
{
	return ListItems.IndexOfByKey(MakeShared<FRareListRowBase>(Item));
}

void URareListView::ClearListItems()
{
	const TArray<TSharedPtr<FRareListRowBase>> Added;
	const TArray<TSharedPtr<FRareListRowBase>> Removed = MoveTemp(ListItems);

	ListItems.Reset();

	RequestRefresh();
}

void URareListView::SetSelectionMode(TEnumAsByte<ESelectionMode::Type> InSelectionMode)
{
	SelectionMode = InSelectionMode;
	if (MyListView)
	{
		MyListView->SetSelectionMode(InSelectionMode);
	}
}

bool URareListView::IsRefreshPending() const
{
	if (MyListView.IsValid())
	{
		return MyListView->IsPendingRefresh();
	}
	return false;
}

void URareListView::ScrollIndexIntoView(int32 Index)
{
	RequestScrollItemIntoView(GetItemAt(Index));
}

void URareListView::SetSelectedIndex(int32 Index)
{
	SetSelectedItemInternal(GetItemAt(Index));
}

void URareListView::NavigateToIndex(int32 Index)
{
	RequestNavigateToItem(GetItemAt(Index));
}

TSharedRef<STableViewBase> URareListView::RebuildListWidget()
{
	return ConstructListView<SListView>();
}

UUserWidget& URareListView::OnGenerateEntryWidgetInternal(TSharedPtr<FRareListRowBase> Item,
                               TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	return GenerateTypedEntry<UUserWidget, SStructTableRow<TSharedPtr<FRareListRowBase>>>(DesiredEntryClass, OwnerTable);
}