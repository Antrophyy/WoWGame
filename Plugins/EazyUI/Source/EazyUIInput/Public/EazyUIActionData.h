#pragma once

#include "UObject/ObjectPtr.h"
#include "EazyUIActionData.generated.h"

class UEazyInputAction;

USTRUCT(BlueprintType, Category=UI)
struct EAZYUIINPUT_API FEazyUIActionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UEazyInputAction> InputAction;

	UPROPERTY(EditAnywhere)
	bool bDisplayInActionBar = false;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bDisplayInActionBar", EditConditionHides))
	FText DisplayName;
	
	// The priority of this action in the action bar. Lower priority are displayed first.
	UPROPERTY(EditAnywhere, meta=(EditCondition="bDisplayInActionBar", EditConditionHides))
	int32 ActionBarPriority = 0;
};
