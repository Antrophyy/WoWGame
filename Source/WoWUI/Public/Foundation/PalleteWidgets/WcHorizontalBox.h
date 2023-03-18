#pragma once

#include "Components/HorizontalBox.h"
#include "WcHorizontalBox.generated.h"

UCLASS()
class WOWUI_API UWcHorizontalBox final : public UHorizontalBox
{
	GENERATED_BODY()

public:

	virtual void SynchronizeProperties() override;

	// Allows you to set spacing between the entries of this vertical box.
	UPROPERTY(EditAnywhere, Category=General)
	float EntrySpacing = 0.f;
};
