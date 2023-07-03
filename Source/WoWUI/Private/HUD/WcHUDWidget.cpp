#include "HUD/WcHUDWidget.h"
#include "LogWoWUI.h"
#include "PrimaryGameLayout.h"
#include "UINativeTags.h"
#include "UITag.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Editor/WidgetCompilerLog.h"
#include "GameFramework/Pawn.h"
#include "HUD/Windows/WcDragWindowOperation.h"
#include "HUD/Windows/Inventory/WcInventoryWidget.h"
#include "Input/CommonUIInputTypes.h"

void UWcHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBindings();
}

void UWcHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetingBehaviorComponent = GetOwningPlayerPawn()->FindComponentByClass<UWcTargetingBehaviorComponent>();
	if (!TargetingBehaviorComponent.IsValid())
	{
		UE_LOG(LogWoWUI, Error, TEXT("[%s::%hs] -> Could not find Targeting Behavior component. Clearing a target by pressing ESC will not work."), *StaticClass()->GetName(), __func__);
	}
}

#if WITH_EDITOR
void UWcHUDWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (PauseScreenClass.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString("{0} has no PauseScreenClass specified in the widget defaults."), FText::FromString(GetName())));
	}
}
#endif // WITH_EDITOR

FReply UWcHUDWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

bool UWcHUDWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

void UWcHUDWidget::RegisterUIActionBindings()
{
	if (!EscapeActionHandle.IsValid())
	{
		EscapeActionHandle = RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
	}

	if (!ToggleInventoryActionHandle.IsValid())
	{
		ToggleInventoryActionHandle = RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_TOGGLE_INVENTORY), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleToggleInventoryAction)));
	}
}

void UWcHUDWidget::HandleEscapeAction()
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
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(GetOwningPlayer()))
	{
		RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_MENUSCREEN, true, PauseScreenClass);
	}
}

void UWcHUDWidget::HandleToggleInventoryAction()
{
	if (!Inventory_Widget)
		return;

	if (OpenedWindows.Contains(Inventory_Widget))
	{
		CollapseWindow(Inventory_Widget);
	}
	else
	{
		ShowWindow(Inventory_Widget);
	}
}

void UWcHUDWidget::CollapseWindow(UWcUserWidget* WindowToCollapse)
{
	if (!WindowToCollapse)
		return;

	if (Inventory_Widget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		UE_LOG(LogWoWUI, Error, TEXT("Attempted to hide a window that is already hidden! Probably wrong handling of window toggle!"));
		return;
	}

	WindowToCollapse->SetVisibility(ESlateVisibility::Collapsed);
	OpenedWindows.Remove(WindowToCollapse);
}

void UWcHUDWidget::ShowWindow(UWcUserWidget* WindowToShow)
{
	if (!WindowToShow)
		return;

	if (Inventory_Widget->GetVisibility() == ESlateVisibility::Visible)
	{
		UE_LOG(LogWoWUI, Error, TEXT("Attempted to show a window that is already shown! Probably wrong handling of window toggle!"));
		return;
	}

	WindowToShow->SetVisibility(ESlateVisibility::Visible);
	OpenedWindows.Push(WindowToShow);
}
