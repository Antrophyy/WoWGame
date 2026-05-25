#include "GameIndicatorLayer.h"

#include "SActorCanvas.h"
#include "Widgets/Layout/SBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameIndicatorLayer)

class SWidget;

/////////////////////////////////////////////////////
// UIndicatorLayer

UGameIndicatorLayer::UGameIndicatorLayer()
{
	bIsVariable = true;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UGameIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyActorCanvas.Reset();
}

TSharedRef<SWidget> UGameIndicatorLayer::RebuildWidget()
{
	if (!IsDesignTime())
	{
		MyActorCanvas = MakeActorCanvas();

		if (MyActorCanvas.IsValid())
		{
			return MyActorCanvas.ToSharedRef();
		}
	}

	// Give it a trivial box, NullWidget isn't safe to use from a UWidget
	return SNew(SBox);
}

TSharedPtr<SActorCanvas> UGameIndicatorLayer::MakeActorCanvas() const
{
	ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	if (ensureMsgf(LocalPlayer, TEXT("Attempting to rebuild a UActorCanvas without a valid LocalPlayer!")))
	{
		const TSharedPtr<SActorCanvas> NewActorCanvas = SNew(SActorCanvas, FLocalPlayerContext(LocalPlayer), &ArrowBrush);
		return NewActorCanvas.ToSharedRef();
	}
	
	return nullptr;
}

