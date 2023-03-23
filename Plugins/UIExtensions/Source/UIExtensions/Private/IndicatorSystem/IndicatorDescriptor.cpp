#include "IndicatorSystem/IndicatorDescriptor.h"
#include "Engine/LocalPlayer.h"
#include "IndicatorSystem/IndicatorManagerComponent.h"

bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2D& ScreenSize, FVector& OutScreenPositionWithDepth) const
{
	TWeakObjectPtr<USceneComponent> Component = IndicatorDescriptor.GetSceneComponent();
	if (Component.IsValid())
	{
		TOptional<FVector> WorldLocation;
		if (IndicatorDescriptor.GetComponentSocketName() != NAME_None)
		{
			WorldLocation = Component->GetSocketTransform(IndicatorDescriptor.GetComponentSocketName()).GetLocation();
		}
		else
		{
			WorldLocation = Component->GetComponentLocation();
		}

		const FVector ProjectWorldLocation = WorldLocation.GetValue() + IndicatorDescriptor.GetWorldPositionOffset();
		const EActorCanvasProjectionMode ProjectionMode = IndicatorDescriptor.GetProjectionMode();

		FBox2D ScreenBox = FBox2D(FVector2D::Zero(), ScreenSize);

		FVector2D UnClampedScreenSpacePosition;
		if (IndicatorDescriptor.GetIsClamped())
		{
			const TSharedPtr<SWidget> CanvasHost = IndicatorDescriptor.GetCanvasHost().Pin();
			if (CanvasHost.IsValid())
			{
				FVector2D CurrentSize = CanvasHost->GetDesiredSize();

				if (IndicatorDescriptor.GetCurrentClampDirection() != EArrowDirection::Left && IndicatorDescriptor.GetCurrentClampDirection() != EArrowDirection::Bottom)
				{
					UnClampedScreenSpacePosition = IndicatorDescriptor.GetUnClampedIndicatorSize() - CurrentSize;
				}
			}
		}
		
		switch (ProjectionMode)
		{
			case EActorCanvasProjectionMode::ComponentPoint:
			{
				if (WorldLocation.IsSet())
				{
					FVector2D OutScreenSpacePosition;
					const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectWorldLocation, OutScreenSpacePosition, &ScreenSize);

					OutScreenSpacePosition -= UnClampedScreenSpacePosition;
					OutScreenSpacePosition.X += IndicatorDescriptor.GetScreenSpaceOffset().X * (bInFrontOfCamera ? 1 : -1);
					OutScreenSpacePosition.Y += IndicatorDescriptor.GetScreenSpaceOffset().Y;

					if (!bInFrontOfCamera && ScreenBox.IsInside(OutScreenSpacePosition))
					{
						const FVector2D CenterToPosition = OutScreenSpacePosition - ScreenBox.GetCenter();
						OutScreenSpacePosition = ScreenBox.GetCenter() + CenterToPosition * ScreenSize.X;
					}

					OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y, FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation));

					return true;
				}

				return false;
			}
			case EActorCanvasProjectionMode::ComponentScreenBoundingBox:
			case EActorCanvasProjectionMode::ActorScreenBoundingBox:
			{
				FBox IndicatorBox;
				if (ProjectionMode == EActorCanvasProjectionMode::ActorScreenBoundingBox)
				{
					IndicatorBox = Component->GetOwner()->GetComponentsBoundingBox();
				}
				else
				{
					IndicatorBox = Component->Bounds.GetBox();
				}

				FVector2D LowerLeft, UpperRight;
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelBoundingBox(InProjectionData, IndicatorBox, LowerLeft, UpperRight, &ScreenSize);

				const FVector& BoundingBoxAnchor = IndicatorDescriptor.GetBoundingBoxAnchor();
				const FVector2D& ScreenSpaceOffset = IndicatorDescriptor.GetScreenSpaceOffset();

				FVector ScreenPositionWithDepth;
				ScreenPositionWithDepth.X -= UnClampedScreenSpacePosition.X;
				ScreenPositionWithDepth.Y -= UnClampedScreenSpacePosition.Y;
				
				ScreenPositionWithDepth.X = FMath::Lerp(LowerLeft.X, UpperRight.X, BoundingBoxAnchor.X) + ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
				ScreenPositionWithDepth.Y = FMath::Lerp(LowerLeft.Y, UpperRight.Y, BoundingBoxAnchor.Y) + ScreenSpaceOffset.Y;
				ScreenPositionWithDepth.Z = FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation);

				const FVector2D ScreenSpacePosition = FVector2D(ScreenPositionWithDepth);
				if (!bInFrontOfCamera && ScreenBox.IsInside(ScreenSpacePosition))
				{
					const FVector2D CenterToPosition = (ScreenSpacePosition - ScreenSize * 0.5f).GetSafeNormal();
					const FVector2D ScreenPositionFromBehind = ScreenSize * 0.5f + CenterToPosition * ScreenSize;
					ScreenPositionWithDepth.X = ScreenPositionFromBehind.X;
					ScreenPositionWithDepth.Y = ScreenPositionFromBehind.Y;
				}

				OutScreenPositionWithDepth = ScreenPositionWithDepth;
				return true;
			}
			case EActorCanvasProjectionMode::ActorBoundingBox:
			case EActorCanvasProjectionMode::ComponentBoundingBox:
			{
				FBox IndicatorBox;
				if (ProjectionMode == EActorCanvasProjectionMode::ActorBoundingBox)
				{
					IndicatorBox = Component->GetOwner()->GetComponentsBoundingBox();
				}
				else
				{
					IndicatorBox = Component->Bounds.GetBox();
				}

				const FVector ProjectBoxPoint = IndicatorBox.GetCenter() + IndicatorBox.GetSize() * (IndicatorDescriptor.GetBoundingBoxAnchor() - FVector(0.5));

				FVector2D OutScreenSpacePosition;
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectBoxPoint, OutScreenSpacePosition, &ScreenSize);

				OutScreenSpacePosition -= UnClampedScreenSpacePosition; 
				OutScreenSpacePosition.X += IndicatorDescriptor.GetScreenSpaceOffset().X * (bInFrontOfCamera ? 1 : -1);
				OutScreenSpacePosition.Y += IndicatorDescriptor.GetScreenSpaceOffset().Y;

				if (!bInFrontOfCamera && ScreenBox.IsInside(OutScreenSpacePosition))
				{
					const FVector2D CenterToPosition = (OutScreenSpacePosition - ScreenSize * 0.5f).GetSafeNormal();
					OutScreenSpacePosition = ScreenSize * 0.5f + CenterToPosition * ScreenSize;
				}

				OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y, FVector::Dist(InProjectionData.ViewOrigin, ProjectBoxPoint));

				return true;
			}
		}
	}

	return false;
}

void UIndicatorDescriptor::SetIndicatorManagerComponent(UIndicatorManagerComponent* InManager)
{
	// Make sure nobody has set this.
	if (ensure(ManagerPtr.IsExplicitlyNull()))
	{
		ManagerPtr = InManager;
	}
}

void UIndicatorDescriptor::UnregisterIndicator()
{
	if (UIndicatorManagerComponent* Manager = ManagerPtr.Get())
	{
		Manager->RemoveIndicator(this);
	}
}
