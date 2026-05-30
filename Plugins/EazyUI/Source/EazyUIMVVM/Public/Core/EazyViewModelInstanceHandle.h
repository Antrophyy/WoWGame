#pragma once

#include "EazyViewModelInstanceHandle.generated.h"

class UEazyViewModelBase;

USTRUCT(Category=MVVM)
struct FEazyViewModelInstanceHandle
{
	GENERATED_BODY()
	
	TWeakObjectPtr<UEazyViewModelBase> ViewModelInstance;
	TArray<TWeakObjectPtr<UObject>> ReferencingObjects;
};
