#include "ExtensionWidgetFactory.h"
#include "Templates/SubclassOf.h"

TSubclassOf<UUserWidget> UExtensionWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
