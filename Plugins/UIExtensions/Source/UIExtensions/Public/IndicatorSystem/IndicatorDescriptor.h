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

public: // Getters

	TWeakObjectPtr<UObject> GetDataObject() const { return DataObject; }

	TWeakObjectPtr<USceneComponent> GetSceneComponent() const { return Component; }

	FName GetComponentSocketName() const { return ComponentSocketName; }

	TSoftClassPtr<UBaseIndicatorWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }

	bool GetIsVisible() const { return GetSceneComponent().IsValid() && bVisible; }

	bool GetIsHitTestable() const { return bIsHitTestable; }

	bool GetIsClamped() const { return bIsClamped; }

	FVector2D GetUnClampedIndicatorSize() const { return UnClampedIndicatorSize; }

	EArrowDirection::Type GetCurrentClampDirection() const { return CurrentClampDirection; }

	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }

	bool GetShouldClampToScreen() const { return bClampToScreen; }

	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }

	int32 GetPriority() const { return Priority; }

	EHorizontalAlignment GetHAlign() const { return HAlignment; }

	EVerticalAlignment GetVAlign() const { return VAlignment; }

	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }

	TWeakPtr<SWidget> GetCanvasHost() const { return CanvasHost; }

	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }

	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }

	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }

	bool CanAutomaticallyRemove() const { return bAutoRemoveWhenIndicatorComponentIsNull && !GetSceneComponent().IsValid(); }

	UIndicatorManagerComponent* GetIndicatorManagerComponent() const { return ManagerPtr.Get(); }

public: // Setters

	void SetDataObject(const TWeakObjectPtr<UObject> InDataObject) { DataObject = InDataObject; }

	void SetSceneComponent(const TWeakObjectPtr<USceneComponent> InComponent) { Component = InComponent; }

	void SetComponentSocketName(const FName& SocketName) { ComponentSocketName = SocketName; }

	void SetIndicatorClass(const TSoftClassPtr<UBaseIndicatorWidget> InIndicatorWidgetClass) { IndicatorWidgetClass = InIndicatorWidgetClass; }

	void SetProjectionMode(const EActorCanvasProjectionMode InProjectionMode) { ProjectionMode = InProjectionMode; }

	// Clamp the indicator to the edge of the screen?
	void SetClampToScreen(const bool bValue) { bClampToScreen = bValue; }

	// Show the arrow if clamping to the edge of the screen?
	void SetShowClampToScreenArrow(const bool bValue) { bShowClampToScreenArrow = bValue; }

	// Allows sorting the indicators (after they are sorted by depth), to allow some group of indicators
	// to always be in front of others.
	void SetPriority(const int32 InPriority) { Priority = InPriority; }

	// Horizontal alignment to the point in space to place the indicator at.
	void SetHAlign(const EHorizontalAlignment InHAlignment) { HAlignment = InHAlignment; }

	// Vertical alignment to the point in space to place the indicator at.
	void SetVAlign(const EVerticalAlignment InVAlignment) { VAlignment = InVAlignment; }

	// The position offset for the indicator in world space.
	void SetWorldPositionOffset(const FVector& Offset) { WorldPositionOffset = Offset; }

	// The position offset for the indicator in screen space.
	void SetScreenSpaceOffset(const FVector2D& Offset) { ScreenSpaceOffset = Offset; }

	void SetBoundingBoxAnchor(const FVector& InBoundingBoxAnchor) { BoundingBoxAnchor = InBoundingBoxAnchor; }

	void SetAutoRemoveWhenIndicatorComponentIsNull(const bool CanAutomaticallyRemove) { bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove; }

	void SetIsVisible(const bool InVisible) { bVisible = InVisible; }

	void SetIsHitTestable(const bool bInHitTestable) { bIsHitTestable = bInHitTestable; }

	void SetIsClamped(const bool InClamped) { bIsClamped = InClamped; }

	void SetCurrentClampDirection(const EArrowDirection::Type ClampDirection) { CurrentClampDirection = ClampDirection; }

	void SetUnClampedIndicatorSize(const FVector2D& Size) { UnClampedIndicatorSize = Size; }

	void SetIndicatorManagerComponent(UIndicatorManagerComponent* InManager);

public:

	void UnregisterIndicator();

	TWeakObjectPtr<UBaseIndicatorWidget> IndicatorWidget;

	FOnWidgetCreated OnWidgetCreated;

private:

	bool bIsClamped = false;

	bool bVisible = true;

	bool bIsHitTestable = false;

	bool bClampToScreen = false;

	bool bShowClampToScreenArrow = false;

	bool bOverrideScreenPosition = false;

	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	EArrowDirection::Type CurrentClampDirection;

	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;

	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;

	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	int32 Priority = 0;

	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);

	FVector2D ScreenSpaceOffset = FVector2D(0, 0);

	FVector WorldPositionOffset = FVector(0, 0, 0);

	FVector2D UnClampedIndicatorSize;

private:

	friend class SActorCanvas;

	TWeakObjectPtr<UObject> DataObject;

	TWeakObjectPtr<USceneComponent> Component;

	FName ComponentSocketName = NAME_None;

	TSoftClassPtr<UBaseIndicatorWidget> IndicatorWidgetClass;

	TWeakObjectPtr<UIndicatorManagerComponent> ManagerPtr;

	TWeakPtr<SWidget> Content;

	TWeakPtr<SWidget> CanvasHost;
};
