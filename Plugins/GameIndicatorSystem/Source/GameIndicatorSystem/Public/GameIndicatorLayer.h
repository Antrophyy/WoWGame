// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Components/Widget.h"

#include "GameIndicatorLayer.generated.h"

class SActorCanvas;
class SWidget;

/** A widget that can be used to display game indicators on the screen.
 * It provides a default arrow brush that can be used for indicators that are clamped to the screen.
 */
UCLASS(ClassGroup=UI)
class GAMEINDICATORSYSTEM_API UGameIndicatorLayer : public UWidget
{
	GENERATED_BODY()

public:
	UGameIndicatorLayer();
	
	/** Default arrow brush to use if UI is clamped to the screen and needs to show an arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush ArrowBrush;

protected:
	// UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End UWidget

	// Can be overriden to create a actor canvas inheriting from SActorCanvas.
	virtual TSharedPtr<SActorCanvas> MakeActorCanvas() const;

protected:
	TSharedPtr<SActorCanvas> MyActorCanvas;
};
