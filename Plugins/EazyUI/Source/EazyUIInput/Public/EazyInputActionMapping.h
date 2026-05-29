#pragma once

#include "Engine/DataAsset.h"
#include "InputMappingContext.h"
#include "EazyInputActionMapping.generated.h"

UCLASS(Blueprintable)
class EAZYUIINPUT_API UEazyInputActionMapping : public UDataAsset
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
