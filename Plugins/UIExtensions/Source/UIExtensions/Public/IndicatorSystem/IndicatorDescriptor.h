#pragma once

#include "SActorCanvas.h"
#include "IndicatorDescriptor.generated.h"

class UBaseIndicatorWidget;
class UIndicatorDescriptor;
class UIndicatorManagerComponent;

struct UIEXTENSIONS_API FIndicatorProjection
{
	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2D& ScreenSize, FVector& OutScreenPositionWithDepth) const;
};

UENUM(BlueprintType)
enum class EActorCanvasProjectionMode : uint8
{
	ComponentPoint,
	ComponentBoundingBox,
	ComponentScreenBoundingBox,
	ActorBoundingBox,
	ActorScreenBoundingBox
};

DECLARE_EVENT_OneParam(UIndicatorDescriptor, FOnWidgetCreated, UBaseIndicatorWidget* CreatedWidget);

/**
 * Describes and controls an active indicator.  It is highly recommended that your widget implements
 * IActorIndicatorWidget so that it can 'bind' to the associated data.
 */
UCLASS(BlueprintType)
class UIEXTENSIONS_API UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

public:

	TWeakObjectPtr<UObject> DataObject;

	TWeakObjectPtr<USceneComponent> SceneComponent;

	FName ComponentSocketName = NAME_None;

	TSoftClassPtr<UBaseIndicatorWidget> IndicatorWidgetClass;

	bool bIsHitTestable = false;

	bool bIsClamped = false;

	bool GetIsVisible() const { return SceneComponent.IsValid() && bVisible; }

	FVector2D UnClampedIndicatorSize;

	EArrowDirection::Type CurrentClampDirection;

	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;

	// Clamp the indicator to the edge of the screen?
	bool bShouldClampToScreen = false;

	// Show the arrow if clamping to the edge of the screen?
	bool bShowClampToScreenArrow = false;

	// Allows sorting the indicators (after they are sorted by depth), to allow some group of indicators
	// to always be in front of others.
	int32 Priority = 0;

	// Horizontal alignment to the point in space to place the indicator at.
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;

	// Vertical alignment to the point in space to place the indicator at.
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	// The position offset for the indicator in world space.
	FVector WorldPositionOffset = FVector(0, 0, 0);

	TWeakPtr<SWidget> GetCanvasHost() const { return CanvasHost; }

	// The position offset for the indicator in screen space.
	FVector2D ScreenSpaceOffset = FVector2D(0, 0);

	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);

	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	bool CanAutomaticallyRemove() const { return bAutoRemoveWhenIndicatorComponentIsNull && !SceneComponent.IsValid(); }

	TWeakObjectPtr<UIndicatorManagerComponent> ManagerComponent;

public:

	void UnregisterIndicator();

	TWeakObjectPtr<UBaseIndicatorWidget> IndicatorWidget;

	FOnWidgetCreated OnWidgetCreated;

private:

	bool bVisible = true;

private:

	friend class SActorCanvas;

	TWeakPtr<SWidget> Content;

	TWeakPtr<SWidget> CanvasHost;
};
