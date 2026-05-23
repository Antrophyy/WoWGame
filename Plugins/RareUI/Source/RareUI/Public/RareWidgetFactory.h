// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "RareWidgetFactory.generated.h"

class UUserWidget;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=UI)
class RAREUI_API URareWidgetFactory : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
