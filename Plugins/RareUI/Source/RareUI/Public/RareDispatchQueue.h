#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Delegates/Delegate.h"

template <typename TPayloadType>
struct TRareDispatchQueue
{
	struct FDispatchQueueElement
	{
		TPayloadType Payload;
		float SpecificDispatchInterval = -1.f;
	};
	
public:

	DECLARE_MULTICAST_DELEGATE_TwoParams(FIgnitePerkDelegate, const TPayloadType& /*PayloadType*/, const float /*DispatchDuration*/);
	// Delegate to be called when a payload is dispatched.
	FIgnitePerkDelegate OnDispatched;

	bool bWaitingForFirstMessage = true;

	// Tick function to be called every frame to process the queue.
	void Tick(const float InDeltaTime)
	{
		if (IsEmpty() && TimeSinceLastDispatch >= DispatchInterval)
		{
			// We're ready to show the first message immediately when one is enqueued.
			bWaitingForFirstMessage = true;
			return;
		}
		
		TimeSinceLastDispatch += InDeltaTime;
		bWaitingForFirstMessage = false;
		
		if (!IsEmpty() && TimeSinceLastDispatch >= DispatchInterval)
		{
			FDispatchQueueElement Element;
			if (MessageQueue.Dequeue(Element))
			{
				ElementsCount--;
				DispatchInterval = Element.SpecificDispatchInterval;
				OnDispatched.Broadcast(Element.Payload, Element.SpecificDispatchInterval);
			}
			
			TimeSinceLastDispatch = 0.f;
		}
	}

	// Enqueue a new payload into the queue. Optionally specify a specific interval for this payload.
	// If no specific interval is provided, the default DispatchInterval will be used.
	void Enqueue(const TPayloadType& InPayload, const float SpecificDispatchInterval = -1.f)
	{
		FDispatchQueueElement NewElement;
		NewElement.Payload = InPayload;
		NewElement.SpecificDispatchInterval = SpecificDispatchInterval < 0.f ? DispatchInterval : SpecificDispatchInterval;

		MessageQueue.Enqueue(NewElement);
		ElementsCount++;
	}

	// Check if the queue is empty.
	bool IsEmpty() const
	{
		return MessageQueue.IsEmpty();
	}

	// Initialize the dispatch queue with a specific interval.
	void SetDispatchInterval(const float InInterval)
	{
		DispatchInterval = InInterval;
	}

	// Clear all elements from the queue.
	void Clear()
	{
		MessageQueue.Empty();
		ElementsCount = 0;
	}

	// Get number of elements currently in the queue.
	int32 Num() const
	{
		return ElementsCount;
	}

private:
	
	TQueue<FDispatchQueueElement> MessageQueue;

	float TimeSinceLastDispatch = 0.f;
	float DispatchInterval = 0.f;
	int32 ElementsCount = 0;
};