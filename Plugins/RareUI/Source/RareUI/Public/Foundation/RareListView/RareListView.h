// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Components/ListViewBase.h"
#include "RareListView.generated.h"

USTRUCT(BlueprintType)
struct FRareListRowBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FStructDesignerPreview : public FRareListRowBase
{
	GENERATED_BODY()
};

 UCLASS(meta = (EntryInterface = "/Script/UMG.UserStructListEntry"))
 class RAREUI_API URareListView : public UListViewBase, public ITypedUMGListView<TSharedPtr<FRareListRowBase>>
 {
  	GENERATED_BODY()

 	IMPLEMENT_TYPED_UMG_LIST(TSharedPtr<FRareListRowBase>, MyListView)
 	
 public:
 	
 	URareListView(const FObjectInitializer& Initializer);
 	
 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
 	virtual void PostLoad() override;

 	/** Set the list of items to display within this listview */
 	template <typename ItemStructT, typename AllocatorType = FDefaultAllocator>
 	void SetListItems(const TArray<ItemStructT, AllocatorType>& InListItems)
 	{
 		ClearListItems();
 		ListItems.Reserve(InListItems.Num());
 		for (const ItemStructT ListItem : InListItems)
 		{
 			TSharedPtr<ItemStructT> ItemPtr = MakeShared<ItemStructT>(ListItem);
 			if (ItemPtr != nullptr)
 			{
 				ListItems.Add(ItemPtr);
 			}
 		}
 		
 		RequestRefresh();
 	}

 	template <typename RowWidgetT = UUserWidget>
	RowWidgetT* GetEntryWidgetFromItem(const FRareListRowBase* Item) const
 	{
 		return Item ? ITypedUMGListView<TSharedPtr<FRareListRowBase>>::GetEntryWidgetFromItem<RowWidgetT>(MakeShared<FRareListRowBase>(Item)) : nullptr;
 	}
 	void SetSelectedItem(const FRareListRowBase& Item);

 	/** Gets the first selected item, if any; recommended that you only use this for single selection lists. */
 	template <typename StructT = FRareListRowBase>
 	StructT* GetSelectedItem() const
 	{
 		return Cast<StructT>(ITypedUMGListView<TSharedPtr<FRareListRowBase>>::GetSelectedItem());
 	}
 	
 	const TArray<TSharedPtr<FRareListRowBase>>& GetListItems() const { return ListItems; }
	
 	/** Adds an the item to the list */
 	void AddItem(const FRareListRowBase& Item);
 	/** Removes an the item from the list */
	void RemoveItem(const FRareListRowBase& Item);
 	/** Returns the item at the given index */
 	TSharedPtr<FRareListRowBase> GetItemAt(int32 Index) const;
 	/** Returns the total number of items */
 	int32 GetNumItems() const;
 	/** Returns the index that the specified item is at. Will return the first found, or -1 for not found */
 	int32 GetIndexForItem(const FRareListRowBase& Item) const;
 	/** Removes all items from the list */
 	void ClearListItems();

 	/** Sets the new selection mode, preserving the current selection where possible. */
 	void SetSelectionMode(TEnumAsByte<ESelectionMode::Type> SelectionMode);
 	/** Returns true if a refresh is pending and the list will be rebuilt on the next tick */
 	bool IsRefreshPending() const;
 	/** Requests that the item at the given index is scrolled into view */
 	void ScrollIndexIntoView(int32 Index);
 	/** Sets the item at the given index as the sole selected item. */
 	void SetSelectedIndex(int32 Index);
 	/** Requests that the item at the given index navigated to, scrolling it into view if needed. */
 	void NavigateToIndex(int32 Index);
 
 protected:

	virtual TSharedRef<STableViewBase> RebuildListWidget() override;
 	
 	virtual UUserWidget& OnGenerateEntryWidgetInternal(TSharedPtr<FRareListRowBase> Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

#if WITH_EDITOR
 	virtual void OnRefreshDesignerItems() override;
#endif

 	
 	template <template<typename> class ListViewT = SListView>
 	TSharedRef<ListViewT<TSharedPtr<FRareListRowBase>>> ConstructListView()
      	{
 		FListViewConstructArgs Args;
 		Args.bAllowFocus = bIsFocusable;
 		Args.SelectionMode = SelectionMode;
 		Args.bClearSelectionOnClick = bClearSelectionOnClick;
 		Args.ConsumeMouseWheel = ConsumeMouseWheel;
 		Args.bReturnFocusToSelection = bReturnFocusToSelection;
 		Args.Orientation = Orientation;
 		Args.ListViewStyle = &WidgetStyle;
 		Args.ScrollBarStyle = &ScrollBarStyle;
 		MyListView = ITypedUMGListView<TSharedPtr<FRareListRowBase>>::ConstructListView<ListViewT>(this, ListItems, Args);
	 
 		return StaticCastSharedRef<ListViewT<TSharedPtr<FRareListRowBase>>>(MyListView.ToSharedRef());
 	}
 	
 protected:

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView, meta = (DisplayName = "Style"))
 	FTableViewStyle WidgetStyle;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	FScrollBarStyle ScrollBarStyle;

 	/** 
	  * The scroll & layout orientation of the list. ListView and TileView only. 
	  * Vertical will scroll vertically and arrange tiles into rows.
	  * Horizontal will scroll horizontally and arrange tiles into columns.
	  */
 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	TEnumAsByte<EOrientation> Orientation;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	TEnumAsByte<ESelectionMode::Type> SelectionMode = ESelectionMode::Single;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	EConsumeMouseWheel ConsumeMouseWheel = EConsumeMouseWheel::WhenScrollingPossible;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	bool bClearSelectionOnClick = false;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	bool bIsFocusable = true;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
 	bool bReturnFocusToSelection = false;

 	
 	
 	TArray<TSharedPtr<FRareListRowBase>> ListItems;
 	
 	TSharedPtr<SListView<TSharedPtr<FRareListRowBase>>> MyListView;

 private:

 	void SetSelectedItemInternal(TSharedPtr<FRareListRowBase> Item);
};
