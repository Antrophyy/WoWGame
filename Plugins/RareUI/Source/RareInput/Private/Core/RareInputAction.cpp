#include "Core/RareInputAction.h"

URareInputAction::URareInputAction(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// All UI actions should be pressed by default. If no trigger is set, the "Triggered" action will fire multiple times.
	// If somebody wants a different trigger, they can change it however they like.
	UInputTriggerPressed* InputTriggerPressed = ObjectInitializer.CreateDefaultSubobject<UInputTriggerPressed>(this, TEXT("DefaultReleasedTrigger"));
	Triggers.Add(InputTriggerPressed);
}
