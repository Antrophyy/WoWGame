#pragma once

#include "Engine/DataAsset.h"
#include "InputMappingContext.h"
#include "RareInputActionMapping.generated.h"

UCLASS(Blueprintable)
class RAREINPUT_API URareInputActionMapping : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InputAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FEnhancedActionKeyMapping> KeyMapping;
	
	UPROPERTY(EditAnywhere, Category = "Meta")
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, Category = "Meta")
	int32 ActionBarPriority = 0; 
};

