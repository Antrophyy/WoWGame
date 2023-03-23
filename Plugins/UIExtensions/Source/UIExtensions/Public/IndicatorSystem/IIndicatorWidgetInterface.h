#pragma once

#include "UObject/Interface.h"
#include "IIndicatorWidgetInterface.generated.h"

class UIndicatorDescriptor;

UINTERFACE(BlueprintType)
class UIEXTENSIONS_API UIndicatorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class UIEXTENSIONS_API IIndicatorWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void BindIndicator(UIndicatorDescriptor* Indicator);

	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void UnbindIndicator(const UIndicatorDescriptor* Indicator);
};
