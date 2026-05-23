// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Components/Image.h"
#include "RareImage.generated.h"

UCLASS()
class RAREUI_API URareImage : public UImage
{
	GENERATED_BODY()

public:
	URareImage();

protected:
	virtual void OnImageStreamingStarted(TSoftObjectPtr<UObject> SoftObject) override;
	virtual void OnImageStreamingComplete(TSoftObjectPtr<UObject> LoadedSoftObject) override;
};
