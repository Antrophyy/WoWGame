#pragma once

#include "InputAction.h"
#include "EazyInputAction.generated.h"

UCLASS(ClassGroup=UI)
class EAZYUIINPUT_API UEazyInputAction : public UInputAction
{
	GENERATED_BODY()
	
public:
	explicit UEazyInputAction(const FObjectInitializer& ObjectInitializer);
};

