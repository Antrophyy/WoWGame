// Copyright (C) Grip Studios. All Rights Reserved

#include "RareWidgetFactory.h"
#include "Templates/SubclassOf.h"

TSubclassOf<UUserWidget> URareWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
