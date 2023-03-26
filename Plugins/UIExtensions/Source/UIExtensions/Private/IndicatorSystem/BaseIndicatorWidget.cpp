#include "IndicatorSystem/BaseIndicatorWidget.h"
#include "LogUIExtensions.h"
#include "Animation/UMGSequencePlayer.h"
#include "IndicatorSystem/IndicatorDescriptor.h"
#include "Animation/WidgetAnimation.h"

void UBaseIndicatorWidget::InitializeIndicatorWidget(const TWeakObjectPtr<UIndicatorDescriptor> InIndicatorDescriptor)
{
	IndicatorDescriptor = InIndicatorDescriptor;

	ActorToAttach = Cast<AActor>(IndicatorDescriptor->DataObject);
	if (!ActorToAttach.IsValid())
	{
		UE_LOG(LogUIExtensions, Error, TEXT("[%s::%hs] -> Actor to attach the interaction widget to is invalid."), *StaticClass()->GetName(), __func__);
	}

	// Play indicator added animation if there is one.
	if (IsValid(IndicatorAddedAnimation))
	{
		PlayAnimation(IndicatorAddedAnimation, 0.f);
	}

	NativeOnIndicatorWidgetInitialized();
}

void UBaseIndicatorWidget::PlayIndicatorRemovedAnimation(const TFunction<void()> OnRemovedAnimationFinished)
{
	const bool bCanPlayAnimations = GetVisibility() != ESlateVisibility::Collapsed && GetVisibility() != ESlateVisibility::Hidden;

	if (IsValid(IndicatorRemovedAnimation) && bCanPlayAnimations)
	{
		RemoveAnimationSequencePlayer = PlayAnimation(IndicatorRemovedAnimation);
		RemoveAnimationSequencePlayer->OnSequenceFinishedPlaying().AddLambda([OnRemovedAnimationFinished, this](UUMGSequencePlayer&)
		{
			OnRemovedAnimationFinished();
		});

		bMarkedForRemoval = true;
	}
	else
	{
		// No valid animation, we can just call the lambda immediately.
		OnRemovedAnimationFinished();
	}
}

void UBaseIndicatorWidget::NativeOnClampedStatusChanged(const bool bIsClamped)
{
	BP_OnClampedStatusChanged(bIsClamped);
}

void UBaseIndicatorWidget::NativeOnIndicatorPreDestruction()
{
	BP_OnIndicatorPreDestruction();
}

void UBaseIndicatorWidget::NativeResetWidgetToDefault()
{
	bMarkedForRemoval = false;
	BP_ResetWidgetToDefault();
}

void UBaseIndicatorWidget::NativeOnIndicatorWidgetInitialized()
{
	BP_OnIndicatorWidgetInitialized();
}

void UBaseIndicatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// During the remove animation it is possible that someone sets the widget to be collapsed or hidden, which means it will pause animations.
	// If the animation is not able to finish, the widget will not be able to clear itself and thus we would have a leak.
	// This takes care of it, if you forcefully hide the widget during the remove animation, we just broadcast animation finished manually.
	OnNativeVisibilityChanged.AddLambda([this](const ESlateVisibility NewVisibility)
	{
		if (!bMarkedForRemoval || !IsValid(RemoveAnimationSequencePlayer))
			return;
		
		if (NewVisibility == ESlateVisibility::Collapsed || NewVisibility == ESlateVisibility::Hidden)
		{
			RemoveAnimationSequencePlayer->OnSequenceFinishedPlaying().Broadcast(*RemoveAnimationSequencePlayer);
		}
	});
}
