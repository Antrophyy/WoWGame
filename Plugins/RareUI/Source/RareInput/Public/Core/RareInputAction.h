#pragma once

#include "InputAction.h"
#include "RareInputAction.generated.h"

UCLASS(ClassGroup=UI)
class RAREINPUT_API URareInputAction : public UInputAction
{
	GENERATED_BODY()
	
public:
	URareInputAction(const FObjectInitializer& ObjectInitializer);
};
