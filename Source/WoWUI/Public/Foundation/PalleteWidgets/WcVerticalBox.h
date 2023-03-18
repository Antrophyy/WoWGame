#pragma once

#include "Components/VerticalBox.h"
#include "WcVerticalBox.generated.h"

UCLASS()
class WOWUI_API UWcVerticalBox final : public UVerticalBox
{
	GENERATED_BODY()

public:

	virtual void SynchronizeProperties() override;

	// Allows you to set spacing between the entries of this vertical box.
	UPROPERTY(EditAnywhere, Category=General)
	float EntrySpacing = 0.f;
};
