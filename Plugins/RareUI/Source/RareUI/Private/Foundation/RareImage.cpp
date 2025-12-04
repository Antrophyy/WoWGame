#include "Foundation/RareImage.h"

URareImage::URareImage()
{
	ThisClass::SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void URareImage::OnImageStreamingStarted(TSoftObjectPtr<UObject> SoftObject)
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void URareImage::OnImageStreamingComplete(TSoftObjectPtr<UObject> LoadedSoftObject)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
