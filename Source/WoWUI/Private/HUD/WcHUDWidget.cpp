#include "HUD/WcHUDWidget.h"
#include "LogWoWUI.h"
#include "PrimaryGameLayout.h"
#include "UINativeTags.h"
#include "UITag.h"
#include "Behaviors/WcTargetingBehaviorComponent.h"
#include "Editor/WidgetCompilerLog.h"
#include "GameFramework/Pawn.h"
#include "Input/CommonUIInputTypes.h"

void UWcHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!EscapeActionHandle.IsValid())
	{
		EscapeActionHandle = RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
	}
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

void UWcHUDWidget::HandleEscapeAction()
{
	if (!TargetingBehaviorComponent.IsValid())
		return;

	// We first check whether we have a target, if we do, we'll just clear the target.
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
