// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareTabListWidgetBase.h"

#include "CommonActionWidget.h"
#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"
#include "CommonLazyWidget.h"
#include "Components/StackBox.h"
#include "Components/StackBoxSlot.h"
#include "Foundation/RareEnhancedActionWidget.h"
#include "Foundation/RareLazyLoadingSwitcher.h"
#include "Foundation/RareLazyWidget.h"

void URareTabListWidgetBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (PrevTabAction)
	{
		PrevTabAction->SetInputAction(PreviousTabInputActionData);
	}

	if (NextTabAction)
	{
		NextTabAction->SetInputAction(NextTabInputActionData);
	}

	if (TabsContainer_StackBox)
	{
		TabsContainer_StackBox->ClearChildren();
		TabsContainer_StackBox->SetOrientation(Orientation);

		if (UStackBoxSlot* StackBoxSlot = Cast<UStackBoxSlot>(TabsContainer_StackBox->Slot))
		{
			if (Orientation == Orient_Horizontal)
			{
				StackBoxSlot->SetHorizontalAlignment(bTakeAvailableSpace ? HAlign_Fill : HAlign_Center);
			}
			else
			{
				StackBoxSlot->SetVerticalAlignment(bTakeAvailableSpace ? VAlign_Fill : VAlign_Center);
			}
		}
	}

	if (Root_StackBox)
	{
		Root_StackBox->SetOrientation(Orientation);
	}

#if WITH_EDITOR
	if (IsDesignTime())
	{
		Debug_CreateTabsDesigner();
	}
#endif // WITH_EDITOR
}

void URareTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetLinkedSwitcher(AnimSwitcher);

	if (IsValid(AnimSwitcher))
	{
		AnimSwitcher->SetIsBeingDestroyed(false);
	}

	if (!IsDesignTime() && GetCachedWidget().IsValid())
	{
		// Don't bother making tabs if we're in the designer or haven't been constructed yet
		SetupPreregisteredTabs();
		SetupDynamicTabs();
	}
}

void URareTabListWidgetBase::NativeDestruct()
{
	if (IsValid(AnimSwitcher))
	{
		AnimSwitcher->SetIsBeingDestroyed(true);
	}

	DynamicTabInfoMap.Empty();

	if (IsValid(TabsContainer_StackBox))
	{
		TabsContainer_StackBox->ClearChildren();
	}

	Super::NativeDestruct();
}

void URareTabListWidgetBase::HandleTabCreation_Implementation(const FName TabId, UCommonButtonBase* TabButton)
{
	UpdateTabVisuals(TabButton);
	UpdatePaddings();
}

void URareTabListWidgetBase::HandleTabRemoval_Implementation(FName TabId, UCommonButtonBase* TabButton)
{
	TabsContainer_StackBox->RemoveChild(TabButton);
}

UWidget* URareTabListWidgetBase::GetActiveTabContent() const
{
	return LinkedSwitcher->GetActiveWidget();
}

void URareTabListWidgetBase::SetTabRegisteredState(const FName TabNameId, const bool bRegistered)
{
	for (FRareTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.TabId == TabNameId)
		{
			TabInfo.bRegistered = bRegistered;
			break;
		}
	}
}

void URareTabListWidgetBase::SelectTabByIndex(const int32 Index, const bool bSuppressClickFeedback)
{
	if (PreregisteredTabInfoArray.IsValidIndex(Index))
	{
		const FName TabId = PreregisteredTabInfoArray[Index].TabId;
		if (UCommonButtonBase* TabButton = GetTabButtonBaseByID(TabId))
		{
			TabButton->SetIsSelected(true, !bSuppressClickFeedback);
		}
	}
}

bool URareTabListWidgetBase::IsFirstTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray[0].TabId;
	}

	return false;
}

bool URareTabListWidgetBase::IsLastTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray.Last().TabId;
	}

	return false;
}

bool URareTabListWidgetBase::IsTabVisible(const FName TabId) const
{
	if (const UCommonButtonBase* Button = GetTabButtonBaseByID(TabId))
	{
		const ESlateVisibility TabVisibility = Button->GetVisibility();
		return TabVisibility == ESlateVisibility::Visible
			|| TabVisibility == ESlateVisibility::HitTestInvisible
			|| TabVisibility == ESlateVisibility::SelfHitTestInvisible;
	}

	return false;
}

int32 URareTabListWidgetBase::GetVisibleTabCount() const
{
	int32 Result = 0;
	const int32 TabCount = GetTabCount();
	for (int32 Index = 0; Index < TabCount; Index++)
	{
		if (IsTabVisible(GetTabIdAtIndex(Index)))
		{
			Result++;
		}
	}

	return Result;
}

void URareTabListWidgetBase::RegisterDynamicTab(FRareTabDescriptor& TabInfo)
{
	if (!TabInfo.bRegistered)
		return;

	DynamicTabInfoMap.Add(TabInfo.TabId, TabInfo);

	if (IsValid(AnimSwitcher))
	{
		PrepareTabForLazyLoad(TabInfo);
	}

	SetupTab(TabInfo);
}

void URareTabListWidgetBase::ToggleTabListEnable(const bool bEnabled)
{
	SetListeningForInput(bEnabled);

	for (int32 i = 0; i < GetTabCount(); i++)
	{
		SetTabEnabled(GetTabIdAtIndex(i), bEnabled);
	}
}

FRareTabDescriptor URareTabListWidgetBase::GetTabInfoById(const FName TabId) const
{
	if (const FRareTabDescriptor* TabInfo = PreregisteredTabInfoArray.FindByPredicate([&](const FRareTabDescriptor& Info) { return Info.TabId == TabId; }))
	{
		return *TabInfo;
	}

	if (const FRareTabDescriptor* TabInfo = DynamicTabInfoMap.Find(TabId))
	{
		return *TabInfo;
	}

	ensureAlwaysMsgf(false, TEXT("Tab with ID %s not found!"), *TabId.ToString());
	return {};
}

void URareTabListWidgetBase::UpdateTabVisuals(UUserWidget* TabButton) const
{
	if (!IsValid(TabButton) || !TabsContainer_StackBox)
		return;

	if (UStackBoxSlot* StackBoxSlot = TabsContainer_StackBox->AddChildToStackBox(TabButton))
	{
		if (bTakeAvailableSpace)
		{
			if (Orientation == Orient_Horizontal)
			{
				StackBoxSlot->SetHorizontalAlignment(HAlign_Center);
			}
			else
			{
				StackBoxSlot->SetVerticalAlignment(VAlign_Center);
			}

			StackBoxSlot->SetSize(ESlateSizeRule::Fill);
		}
	}
}

#if WITH_EDITOR
void URareTabListWidgetBase::Debug_CreateTabsDesigner()
{
	// Possibly apply same principle as tab content to permanently create Tabs in widget

	if (TabsContainer_StackBox)
	{
		TabsContainer_StackBox->ClearChildren();
	}

	if (PreregisteredTabInfoArray.IsEmpty())
	{
		if (IsValid(DebugTab_Class))
		{
			for (int32 i = 0; i < DebugTabCount; ++i)
			{
				UCommonButtonBase* Button = CreateWidget<UCommonButtonBase>(GetRootWidget(), DebugTab_Class);
				UpdateTabVisuals(Button);
				UpdatePaddings();
			}
		}
	}
	else
	{
		for (FRareTabDescriptor TabInfo : PreregisteredTabInfoArray)
		{
			if (!IsValid(TabInfo.TabButtonType))
				continue;
			
			UCommonButtonBase* Button = CreateWidget<UCommonButtonBase>(GetRootWidget(), TabInfo.TabButtonType);
			HandleTabCreation_Implementation(TabInfo.TabId, Button);
		}
	}
}
#endif // WITH_EDITOR

void URareTabListWidgetBase::SetupPreregisteredTabs()
{
	for (FRareTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (IsValid(AnimSwitcher))
		{
			PrepareTabForLazyLoad(TabInfo);
		}

		SetupTab(TabInfo);
	}
}

void URareTabListWidgetBase::SetupDynamicTabs()
{
	for (TPair<FName, FRareTabDescriptor>& Pair : DynamicTabInfoMap)
	{
		FRareTabDescriptor& TabInfo = Pair.Value;

		if (IsValid(AnimSwitcher))
		{
			PrepareTabForLazyLoad(TabInfo);
		}

		SetupTab(Pair.Value);
	}
}

void URareTabListWidgetBase::SetupTab(const FRareTabDescriptor& TabInfo)
{
	if (!TabInfo.bRegistered)
		return;

	// If the tab is not already registered, register it.
	if (GetTabButtonBaseByID(TabInfo.TabId) == nullptr)
	{
		RegisterTab(TabInfo.TabId, TabInfo.TabButtonType, TabInfo.LazyContent);
	}
}

void URareTabListWidgetBase::PrepareTabForLazyLoad(FRareTabDescriptor& TabInfo)
{
	URareLazyWidget* LazyWidget = NewObject<URareLazyWidget>(this);
	LazyWidget->SetContentClass(TabInfo.TabContentType);
	LazyWidget->bShouldSyncLoad = TabInfo.bShouldSyncLoad;

	AnimSwitcher->AddChild(LazyWidget);

	TabInfo.LazyContent = LazyWidget;
}

void URareTabListWidgetBase::UpdatePaddings() const
{
	if (TabsContainer_StackBox)
	{
		// We update paddings around every tab in the container.
		for (int32 i = 0; i < TabsContainer_StackBox->GetChildrenCount(); ++i)
		{
			UUserWidget* Tab = Cast<UUserWidget>(TabsContainer_StackBox->GetChildAt(i));

			// We only want to add padding to the tabs that are not the last one in the list (i.e., the last tab should not have padding).
			const bool bIsLastTab = i == TabsContainer_StackBox->GetChildrenCount() - 1;
			if (bIsLastTab)
				continue;

			if (Orientation == Orient_Horizontal)
			{
				Tab->SetPadding(FMargin(0, 0, TabsSpacing, 0));
			}
			else
			{
				Tab->SetPadding(FMargin(0, 0, 0, TabsSpacing));
			}
		}
	}

	// We update padding for action widgets from the tab container.
	if (NextTabAction)
	{
		if (UStackBoxSlot* ActionSlot = Cast<UStackBoxSlot>(NextTabAction->Slot))
		{
			if (Orientation == Orient_Horizontal)
			{
				ActionSlot->SetPadding(FMargin(ActionsSpacing, 0, 0, 0));
			}
			else
			{
				ActionSlot->SetPadding(FMargin(0, ActionsSpacing, 0, 0));
			}
		}
	}

	if (PrevTabAction)
	{
		if (UStackBoxSlot* ActionSlot = Cast<UStackBoxSlot>(PrevTabAction->Slot))
		{
			if (Orientation == Orient_Horizontal)
			{
				ActionSlot->SetPadding(FMargin(0, 0, ActionsSpacing, 0));
			}
			else
			{
				ActionSlot->SetPadding(FMargin(0, 0, 0, ActionsSpacing));
			}
		}
	}
}
