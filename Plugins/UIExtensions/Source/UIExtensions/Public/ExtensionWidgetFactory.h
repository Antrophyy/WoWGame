#pragma once

#include "ExtensionWidgetFactory.generated.h"

class UUserWidget;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=UI)
class UIEXTENSIONS_API UExtensionWidgetFactory : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
