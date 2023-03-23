#pragma once

#include "AsyncMixin.h"
#include "Blueprint/UserWidgetPool.h"
#include "SceneView.h"

class UIndicatorManagerComponent;
class UIndicatorDescriptor;

namespace EArrowDirection
{
	enum Type
	{
		Left,
		Top,
		Right,
		Bottom,
		MAX
	};
}

class UIEXTENSIONS_API SActorCanvas final : public SPanel, public FAsyncMixin, public FGCObject
{
public:
	/** ActorCanvas-specific slot class */
	class FSlot final : public TSlotBase<FSlot>
	{
	public:
		FSlot(UIndicatorDescriptor* InIndicator)
			: TSlotBase<FSlot>()
			  , Indicator(InIndicator)
			  , ScreenPosition(FVector2D::ZeroVector)
			  , Depth(0)
			  , Priority(0.f)
			  , bIsIndicatorVisible(true)
			  , bInFrontOfCamera(true)
			  , bHasValidScreenPosition(false)
			  , bDirty(true)
			  , bWasIndicatorClamped(false)
			  , bWasIndicatorClampedStatusChanged(false) { }

		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
		SLATE_SLOT_END_ARGS()

		using TSlotBase<FSlot>::Construct;

		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }

		void SetIsIndicatorVisible(const bool bVisible)
		{
			if (bIsIndicatorVisible != bVisible)
			{
				bIsIndicatorVisible = bVisible;
				bDirty = true;
			}

			RefreshVisibility();
		}

		FVector2D GetScreenPosition() const { return ScreenPosition; }

		void SetScreenPosition(const FVector2D& InScreenPosition)
		{
			if (ScreenPosition != InScreenPosition)
			{
				ScreenPosition = InScreenPosition;
				bDirty = true;
			}
		}

		double GetDepth() const { return Depth; }

		void SetDepth(const double InDepth)
		{
			if (Depth != InDepth)
			{
				Depth = InDepth;
				bDirty = true;
			}
		}

		int32 GetPriority() const { return Priority; }

		void SetPriority(const int32 InPriority)
		{
			if (Priority != InPriority)
			{
				Priority = InPriority;
				bDirty = true;
			}
		}

		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }

		void SetInFrontOfCamera(const bool bInFront)
		{
			if (bInFrontOfCamera != bInFront)
			{
				bInFrontOfCamera = bInFront;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }

		void SetHasValidScreenPosition(const bool bValidScreenPosition)
		{
			if (bHasValidScreenPosition != bValidScreenPosition)
			{
				bHasValidScreenPosition = bValidScreenPosition;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool IsDirty() const { return bDirty; }

		void ClearDirtyFlag()
		{
			bDirty = false;
		}

		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }

		void SetWasIndicatorClamped(const bool bWasClamped) const
		{
			if (bWasClamped != bWasIndicatorClamped)
			{
				bWasIndicatorClamped = bWasClamped;
				bWasIndicatorClampedStatusChanged = true;
			}
		}

		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }

		void ClearIndicatorClampedStatusChangedFlag() const
		{
			bWasIndicatorClampedStatusChanged = false;
		}

	private:
		void RefreshVisibility() const
		{
			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;
			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
		}

		//Kept Alive by SActorCanvas::AddReferencedObjects
		UIndicatorDescriptor* Indicator;
		FVector2D ScreenPosition;
		double Depth;
		int32 Priority;

		uint8 bIsIndicatorVisible : 1;
		uint8 bInFrontOfCamera : 1;
		uint8 bHasValidScreenPosition : 1;
		uint8 bDirty : 1;

		/** 
		 * Cached & frame-deferred value of whether the indicator was visually screen clamped last frame or not; 
		 * Semi-hacky mutable implementation as it is cached during a const paint operation
		 */
		mutable uint8 bWasIndicatorClamped : 1;
		mutable uint8 bWasIndicatorClampedStatusChanged : 1;

		friend class SActorCanvas;
	};

	/** ActorCanvas-specific slot class */
	class FArrowSlot final : public TSlotBase<FArrowSlot> { };

	/** Begin the arguments for this slate widget */
	SLATE_BEGIN_ARGS(SActorCanvas)
		{
			_Visibility = EVisibility::HitTestInvisible;
		}

		/** Indicates that we have a slot that this widget supports */
		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)

		/** This always goes at the end */
	SLATE_END_ARGS()

	SActorCanvas()
		: CanvasChildren(this)
		  , ArrowChildren(this)
		  , AllChildren(this)
	{
		AllChildren.AddChildren(CanvasChildren);
		AllChildren.AddChildren(ArrowChildren);
	}

	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InLocalPlayerContext, const FSlateBrush* InActorCanvasArrowBrush);

	// SWidget Interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }
	virtual FChildren* GetChildren() override { return &AllChildren; }
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	// End SWidget

	void SetDrawElementsInOrder(const bool bInDrawElementsInOrder) { bDrawElementsInOrder = bInDrawElementsInOrder; }

	virtual FString GetReferencerName() const override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	void OnIndicatorAdded(UIndicatorDescriptor* Indicator);
	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator);

	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator);
	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator);

	void RemoveCanvasSlot(UIndicatorDescriptor* Indicator);

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	FScopedWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator);
	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget);

	void SetShowAnyIndicators(bool bIndicators);
	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime);

	/** Helper function for calculating the offset */
	static void GetOffsetAndSize(UIndicatorDescriptor* Indicator,
	                             FVector2D& OutSize,
	                             FVector2D& OutOffset,
	                             FVector2D& OutPaddingMin,
	                             FVector2D& OutPaddingMax);

	void UpdateActiveTimer();

private:
	TArray<UIndicatorDescriptor*> AllIndicators;
	TArray<UIndicatorDescriptor*> InactiveIndicators;

	FLocalPlayerContext LocalPlayerContext;
	TWeakObjectPtr<UIndicatorManagerComponent> IndicatorComponentPtr;

	/** All the slots in this canvas */
	TPanelChildren<FSlot> CanvasChildren;
	mutable TPanelChildren<FArrowSlot> ArrowChildren;
	FCombinedChildren AllChildren;

	FUserWidgetPool IndicatorPool;

	const FSlateBrush* ActorCanvasArrowBrush = nullptr;

	mutable int32 NextArrowIndex = 0;
	mutable int32 ArrowIndexLastUpdate = 0;

	/** Whether to draw elements in the order they were added to canvas. Note: Enabling this will disable batching and will cause a greater number of drawcalls */
	bool bDrawElementsInOrder = false;

	bool bShowAnyIndicators = false;

	mutable TOptional<FGeometry> OptionalPaintGeometry;

	TSharedPtr<FActiveTimerHandle> TickHandle;
};
