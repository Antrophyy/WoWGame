#pragma once

#include "CommonLazyImage.h"
#include "EazyImage.generated.h"

UCLASS()
class EAZYUI_API UEazyImage : public UCommonLazyImage
{
	GENERATED_BODY()

public:
	UEazyImage();

protected:
	virtual void OnImageStreamingStarted(TSoftObjectPtr<UObject> SoftObject) override;
	virtual void OnImageStreamingComplete(TSoftObjectPtr<UObject> LoadedSoftObject) override;
};
