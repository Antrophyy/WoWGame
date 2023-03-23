#pragma once

#include "Components/Widget.h"
#include "IndicatorLayer.generated.h"

class SActorCanvas;

UCLASS()
class UIEXTENSIONS_API UIndicatorLayer : public UWidget
{
	GENERATED_BODY()

public:
	UIndicatorLayer(const FObjectInitializer& ObjectInitializer);

	/** Default arrow brush to use if UI is clamped to the screen and needs to show an arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush ArrowBrush;

protected:
	// UWidget interface
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End UWidget

protected:
	TSharedPtr<SActorCanvas> MyActorCanvas;
};
