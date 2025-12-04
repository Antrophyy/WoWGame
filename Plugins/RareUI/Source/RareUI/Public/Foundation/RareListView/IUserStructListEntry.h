#pragma once

#include "Blueprint/IUserListEntry.h"
#include "RareListView.h"
#include "IUserStructListEntry.generated.h"

/**
 * Required interface for any UUserWidget class to be usable as entry widget in a stock UMG list view - ListView, TileView, and TreeView
 * Provides a change event and getter for the item the entry is assigned to represent by the owning list view (in addition to functionality from IUserListEntry)
 */
UINTERFACE(MinimalAPI)
class UUserStructListEntry : public UUserListEntry
{
	GENERATED_UINTERFACE_BODY()
};

class RAREUI_API IUserStructListEntry : public IUserListEntry
{
	GENERATED_IINTERFACE_BODY()

public:
	/** Returns the FRareListRowBase item in the owning UListView that this entry currently represents */
	template <typename ItemStructT = FRareListRowBase>
	TSharedPtr<ItemStructT> GetListItem() const
	{
		static_assert(TIsDerivedFrom<ItemStructT, FRareListRowBase>::IsDerived, "Items represented by an StructListEntry are always expected to be of type FRareListRowBase.");
		return Cast<ItemStructT>(GetListItemInternal());
	}
	
protected:
	/** Follows the same pattern as the NativeOn[X] methods in UUserWidget - super calls are expected in order to route the event to BP. */
	virtual void NativeOnListItemSet(TSharedPtr<FRareListRowBase> ListItem);
	
private:
	TSharedPtr<FRareListRowBase> GetListItemInternal() const;

	template <typename> friend class SObjectTableRow;
	static void SetListItem(UUserWidget& ListEntryWidget, TSharedPtr<FRareListRowBase> ListItem);
};

/** Static library to supply "for free" functionality to widgets that implement IUserListEntry */
UCLASS()
class RAREUI_API UUserStructListEntryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 
	 * Returns the item in the owning list view that this entry is currently assigned to represent. 
	 * @param UserStructListEntry Note: Visually not transmitted, but this defaults to "self". No need to hook up if calling internally.
	 */
	static TSharedPtr<FRareListRowBase> GetListItem(TScriptInterface<IUserStructListEntry> UserStructListEntry);
};

