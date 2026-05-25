#pragma once

#include "UObject/ObjectPtr.h"
#include "RareUIActionData.generated.h"

class URareInputAction;

USTRUCT(BlueprintType, Category=UI)
struct RAREINPUT_API FRareUIActionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<URareInputAction> InputAction;

	UPROPERTY(EditAnywhere)
	bool bDisplayInActionBar = false;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bDisplayInActionBar", EditConditionHides))
	FText DisplayName;
	
	// The priority of this action in the action bar. Lower priority are displayed first.
	UPROPERTY(EditAnywhere, meta=(EditCondition="bDisplayInActionBar", EditConditionHides))
	int32 ActionBarPriority = 0;
};
