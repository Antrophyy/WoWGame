
#pragma once

#include "CommonLazyImage.h"
#include "RareImage.generated.h"

UCLASS()
class RAREUI_API URareImage : public UCommonLazyImage
{
	GENERATED_BODY()

public:
	URareImage();

protected:
	virtual void OnImageStreamingStarted(TSoftObjectPtr<UObject> SoftObject) override;
	virtual void OnImageStreamingComplete(TSoftObjectPtr<UObject> LoadedSoftObject) override;
};
