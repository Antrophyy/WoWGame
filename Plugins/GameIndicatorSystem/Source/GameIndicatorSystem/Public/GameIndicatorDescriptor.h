
#pragma once

#include "Types/SlateEnums.h"
#include "GameIndicatorDescriptor.generated.h"

class SWidget;

class UGameIndicatorDescriptor;
class UGameIndicatorManagerComponent;
class UGameBaseIndicatorWidget;
class USceneComponent;

struct FSceneViewProjectionData;

struct FIndicatorProjection
{
	bool Project(const UGameIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth) const;
};

UENUM(BlueprintType, Category=UI)
enum class EActorCanvasProjectionMode : uint8
{
	ComponentPoint,
	ComponentBoundingBox,
	ComponentScreenBoundingBox,
	ActorBoundingBox,
	ActorScreenBoundingBox
};

DECLARE_EVENT_OneParam(UIndicatorDescriptor, FOnWidgetCreated, UGameBaseIndicatorWidget* CreatedWidget);

/**
 * Describes and controls an active indicator.
 */
UCLASS(BlueprintType, ClassGroup=UI)
class GAMEINDICATORSYSTEM_API UGameIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

public:

	// The name of the socket on the component to attach the indicator to.
	FName ComponentSocketName = NAME_None;

	// Any object that this indicator is associated with. You will have access to this object in the indicator widget.
	TWeakObjectPtr<UObject> DataObject;

	// The component that this indicator is attached to. If this is null, the indicator will not be visible.
	TWeakObjectPtr<USceneComponent> SceneComponent;
	
    // The class of the widget to use for this indicator.
	TSoftClassPtr<UGameBaseIndicatorWidget> IndicatorWidgetClass;

	bool bIsHitTestable = false;
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;
	bool bIsClamped = false;
	
	// Clamp the indicator to the edge of the screen?
	bool bShouldClampToScreen = false;
	
	// Show the arrow if clamping to the edge of the screen?
	bool bShowClampToScreenArrow = false;
	
	// Allows sorting the indicators (after they are sorted by depth), to allow some group of indicators
	// to always be in front of others.
	int32 Priority = 0;

	// The projection mode to use for this indicator.
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;

	// Horizontal alignment to the point in space to place the indicator at.
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;

	// Vertical alignment to the point in space to place the indicator at.
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	// The anchor point for the bounding box of the component or actor.
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);

	// The position offset for the indicator in world space.
	FVector WorldPositionOffset = FVector(0, 0, 0);

	// The position offset for the indicator in screen space.
	FVector2D ScreenSpaceOffset = FVector2D(0, 0);

	// This variable is meant to be used in the offcase that the scene component is not valid to have the indicator be attached to it
	// Thus this position value must be constantly updated
	TOptional<FVector> WorldLocationOverride = FVector(0, 0, 0);

public:

	bool CanAutomaticallyRemove() const { return bAutoRemoveWhenIndicatorComponentIsNull && !SceneComponent.IsValid(); }
	bool GetIsVisible() const { return (SceneComponent.IsValid() || WorldLocationOverride.IsSet()) && bVisible; }

	TWeakPtr<SWidget> GetCanvasHost() const { return CanvasHost; }
	
public:
	// Layout Properties
	//=======================

	void SetDesiredVisibility(const bool InVisible){ bVisible = InVisible; }

public: // Indicator Handling
	
	TWeakObjectPtr<UGameIndicatorManagerComponent> ManagerComponent;
	TWeakObjectPtr<UGameBaseIndicatorWidget> IndicatorWidget;

	FOnWidgetCreated OnWidgetCreated;
	
	void UnregisterIndicator();

private:

	friend class SActorCanvas;
	
	bool bVisible = true;
	
	TWeakPtr<SWidget> Content;
	TWeakPtr<SWidget> CanvasHost;
};
