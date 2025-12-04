#include "Game/HUD/WHUDScreen.h"

#include "LogWoWUI.h"
#include "RarePrimaryGameLayout.h"
#include "UINativeTags.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/HitResult.h"
#include "GameFramework/Pawn.h"
#include "Game/HUD/Modals/WcDragWindowOperation.h"
#include "Game/HUD/Modals/PlayerInventory/WInventoryWidget.h"
#include "Game/HUD/Modals/QuestLog/WQuestLogWidget.h"
#include "Input/CommonUIInputTypes.h"

void UWHUDScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(InvokePauseMenuActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleInvokePauseMenuAction)));
	RegisterUIActionBinding(FBindUIActionArgs(TogglePlayerInventoryActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleTogglePlayerInventoryAction)));
	RegisterUIActionBinding(FBindUIActionArgs(ToggleQuestLogActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleToggleQuestLogAction)));
}

void UWHUDScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	TargetingBehaviorComponent = GetOwningPlayerPawn()->FindComponentByClass<UWcTargetingBehaviorComponent>();
	if (!TargetingBehaviorComponent.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not find Targeting Behavior component. Clearing a target by pressing ESC will not work."), *StaticClass()->GetName(), __func__);
	}
}

FReply UWHUDScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector MouseLocation;
	FVector MouseDirection;
	GetOwningPlayer()->DeprojectMousePositionToWorld(OUT MouseLocation, OUT MouseDirection);

	const FVector TraceEnd = MouseLocation + MouseDirection * 10'000.f;

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OUT HitResult, MouseLocation, TraceEnd, ECC_Visibility, FCollisionQueryParams());
	if (bHit)
	{
		if (APawn* Pawn = Cast<APawn>(HitResult.GetActor()))
		{
			if (TargetingBehaviorComponent.IsValid())
			{
				TargetingBehaviorComponent->ChangeTarget(Pawn);
			}
		}
	}

	return FReply::Unhandled();
}

bool UWHUDScreen::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	const UWcDragWindowOperation* DragWindowResult = Cast<UWcDragWindowOperation>(InOperation);

	if (!IsValid(DragWindowResult))
	{
		UE_LOG(LogWoWUI, Error, TEXT("Could not drop the window to a new location. Drag and dropd function failed."));
		return false;
	}

	const FVector2f DragWindowOffset = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());

	if (!IsValid(DragWindowResult->WidgetReference))
		return false;

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DragWindowResult->WidgetReference->Slot);
	if (!IsValid(CanvasSlot))
		return false;

	const FVector2D AlignmentOffset = CanvasSlot->GetAlignment() * CanvasSlot->Content->GetDesiredSize();
	const FVector2D DragWindowOffsetResult = (FVector2D)DragWindowOffset - (FVector2D)DragWindowResult->DragOffset - InGeometry.GetLocalSize() * CanvasSlot->GetAnchors().Minimum + AlignmentOffset;

	CanvasSlot->SetPosition(DragWindowOffsetResult);
	DragWindowResult->WidgetReference->SetVisibility(ESlateVisibility::Visible);
	return true;
}

void UWHUDScreen::HandleInvokePauseMenuAction()
{
	if (!TargetingBehaviorComponent.IsValid())
		return;

	// Check whether we have any windows opened (inventory, talent window etc.), if we do, deactivate the latest opened.
	if (!OpenedWindows.IsEmpty())
	{
		CollapseWindow(OpenedWindows.Pop());
		return;
	}

	// Check whether we have a valid target selected, if we do, we'll just clear the target.
	if (TargetingBehaviorComponent->GetCurrentTarget().IsValid())
	{
		TargetingBehaviorComponent->ChangeTarget(nullptr);
		return;
	}

	// If we don't have a target, we're free to go to the escape menu.
	if (URarePrimaryGameLayout* RootLayout = URarePrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayer()))
	{
		RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_MENUSCREEN, true, PauseScreen_Class);
	}
}

void UWHUDScreen::HandleTogglePlayerInventoryAction()
{
	if (!IsValid(PlayerInventory_Widget))
		return;

	if (OpenedWindows.Contains(PlayerInventory_Widget))
	{
		CollapseWindow(PlayerInventory_Widget);
	}
	else
	{
		ShowWindow(PlayerInventory_Widget);
	}
}

void UWHUDScreen::HandleToggleQuestLogAction()
{
	if (!IsValid(QuestLog_Widget))
		return;

	if (OpenedWindows.Contains(QuestLog_Widget))
	{
		CollapseWindow(QuestLog_Widget);
	}
	else
	{
		ShowWindow(QuestLog_Widget);
	}
}

void UWHUDScreen::CollapseWindow(UWUserWidget* WindowToCollapse)
{
	if (!IsValid(WindowToCollapse))
		return;

	if (WindowToCollapse->GetVisibility() == ESlateVisibility::Collapsed)
	{
		UE_LOG(LogWoWUI, Error, TEXT("Attempted to hide a window that is already hidden! Probably wrong handling of window toggle!"));
		return;
	}

	WindowToCollapse->SetVisibility(ESlateVisibility::Collapsed);
	OpenedWindows.Remove(WindowToCollapse);
}

void UWHUDScreen::ShowWindow(UWUserWidget* WindowToShow)
{
	if (!IsValid(WindowToShow))
		return;

	if (PlayerInventory_Widget->GetVisibility() == ESlateVisibility::Visible)
	{
		UE_LOG(LogWoWUI, Error, TEXT("Attempted to show a window that is already shown! Probably wrong handling of window toggle!"));
		return;
	}

	WindowToShow->SetVisibility(ESlateVisibility::Visible);
	OpenedWindows.Push(WindowToShow);
}
