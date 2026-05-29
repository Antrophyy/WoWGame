#include "Foundation/EazyImage.h"

UEazyImage::UEazyImage()
{
	ThisClass::SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UEazyImage::OnImageStreamingStarted(TSoftObjectPtr<UObject> SoftObject)
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UEazyImage::OnImageStreamingComplete(TSoftObjectPtr<UObject> LoadedSoftObject)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
