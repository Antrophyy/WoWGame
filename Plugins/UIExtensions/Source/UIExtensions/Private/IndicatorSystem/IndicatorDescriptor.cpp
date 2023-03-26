#include "IndicatorSystem/IndicatorDescriptor.h"
#include "Engine/LocalPlayer.h"
#include "IndicatorSystem/IndicatorManagerComponent.h"

bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2D& ScreenSize, FVector& OutScreenPositionWithDepth) const
{
	TWeakObjectPtr<USceneComponent> Component = IndicatorDescriptor.SceneComponent;
	if (Component.IsValid())
	{
		TOptional<FVector> WorldLocation;
		if (IndicatorDescriptor.ComponentSocketName != NAME_None)
		{
			WorldLocation = Component->GetSocketTransform(IndicatorDescriptor.ComponentSocketName).GetLocation();
		}
		else
		{
			WorldLocation = Component->GetComponentLocation();
		}

		const FVector ProjectWorldLocation = WorldLocation.GetValue() + IndicatorDescriptor.WorldPositionOffset;
		const EActorCanvasProjectionMode ProjectionMode = IndicatorDescriptor.ProjectionMode;

		FBox2D ScreenBox = FBox2D(FVector2D::Zero(), ScreenSize);

		FVector2D UnClampedScreenSpacePosition;
		if (IndicatorDescriptor.bIsClamped)
		{
			const TSharedPtr<SWidget> CanvasHost = IndicatorDescriptor.GetCanvasHost().Pin();
			if (CanvasHost.IsValid())
			{
				FVector2D CurrentSize = CanvasHost->GetDesiredSize();

				if (IndicatorDescriptor.CurrentClampDirection != EArrowDirection::Left && IndicatorDescriptor.CurrentClampDirection != EArrowDirection::Bottom)
				{
					UnClampedScreenSpacePosition = IndicatorDescriptor.UnClampedIndicatorSize - CurrentSize;
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
					OutScreenSpacePosition.X += IndicatorDescriptor.ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
					OutScreenSpacePosition.Y += IndicatorDescriptor.ScreenSpaceOffset.Y;

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

				const FVector& BoundingBoxAnchor = IndicatorDescriptor.BoundingBoxAnchor;
				const FVector2D& ScreenSpaceOffset = IndicatorDescriptor.ScreenSpaceOffset;

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

				const FVector ProjectBoxPoint = IndicatorBox.GetCenter() + IndicatorBox.GetSize() * (IndicatorDescriptor.BoundingBoxAnchor - FVector(0.5));

				FVector2D OutScreenSpacePosition;
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectBoxPoint, OutScreenSpacePosition, &ScreenSize);

				OutScreenSpacePosition -= UnClampedScreenSpacePosition; 
				OutScreenSpacePosition.X += IndicatorDescriptor.ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
				OutScreenSpacePosition.Y += IndicatorDescriptor.ScreenSpaceOffset.Y;

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

void UIndicatorDescriptor::UnregisterIndicator()
{
	if (UIndicatorManagerComponent* Manager = ManagerComponent.Get())
	{
		Manager->RemoveIndicator(this);
	}
}
