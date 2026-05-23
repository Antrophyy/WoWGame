// Copyright (C) Grip Studios. All Rights Reserved

#include "GameBaseIndicatorWidget.h"
#include "GameIndicatorDescriptor.h"
#include "LogGameIndicatorSystem.h"
#include "Animation/WidgetAnimation.h"

void UGameBaseIndicatorWidget::InitializeIndicatorWidget(const TWeakObjectPtr<UGameIndicatorDescriptor> InIndicatorDescriptor)
{
	IndicatorDescriptor = InIndicatorDescriptor;

	DataObject = IndicatorDescriptor->DataObject;
	if (!DataObject.IsValid())
	{
		UE_LOG(LogGameIndicatorSystem, Error, TEXT("[%s::%hs] -> DataObject is invalid."), *StaticClass()->GetName(), __func__);
	}

	// Play indicator added animation if there is one.
	if (IsValid(IndicatorAdded_Anim))
	{
		PlayAnimationForward(IndicatorAdded_Anim, 0.f);
	}

	NativeOnIndicatorWidgetInitialized();
}

void UGameBaseIndicatorWidget::PlayIndicatorRemovedAnimation()
{
	const bool bCanPlayAnimations = GetVisibility() != ESlateVisibility::Collapsed && GetVisibility() != ESlateVisibility::Hidden;

	if (IsValid(IndicatorRemoved_Anim) && bCanPlayAnimations)
	{
		PlayAnimationForward(IndicatorRemoved_Anim);
		bMarkedForRemoval = true;
	}
	else
	{
		// No valid animation, we can just call the broadcast immediately.
		OnIndicatorRemovedAnimationFinished.Broadcast(this);
	}
}

void UGameBaseIndicatorWidget::NativeOnClampedStatusChanged(const bool bIsClamped)
{
	// Hide indicator widget once it has been clamped
	SetVisibility(bIsClamped && bCollapseWhenClamped ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UGameBaseIndicatorWidget::HandleIndicatorRemovedAnimationFinished()
{
	OnIndicatorRemovedAnimationFinished.Broadcast(this);
}

void UGameBaseIndicatorWidget::SetVisibility(const ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	// During the remove animation it is possible that someone sets the widget to be collapsed or hidden, which means it will pause animations.
	// If the animation is not able to finish, the widget will not be able to clear itself, and thus we would have a leak.
	// This takes care of it, if you forcefully hide the widget during the remove animation, we just broadcast animation finished manually.
	
	if (!bMarkedForRemoval)
	{
		return;
	}

	if (InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden)
	{
		HandleIndicatorRemovedAnimationFinished();
	}
}

void UGameBaseIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FWidgetAnimationDynamicEvent OnRemovedAnimationFinished;
	OnRemovedAnimationFinished.BindDynamic(this, &ThisClass::HandleIndicatorRemovedAnimationFinished);
	
	BindToAnimationFinished(IndicatorRemoved_Anim, OnRemovedAnimationFinished);
}

void UGameBaseIndicatorWidget::NativeDestruct()
{
	UnbindAllFromAnimationFinished(IndicatorRemoved_Anim);
	
	Super::NativeDestruct();
}
