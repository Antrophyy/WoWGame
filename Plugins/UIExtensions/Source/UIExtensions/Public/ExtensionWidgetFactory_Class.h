#pragma once

#include "ExtensionWidgetFactory.h"
#include "ExtensionWidgetFactory_Class.generated.h"

UCLASS(ClassGroup=UI)
class UIEXTENSIONS_API UExtensionWidgetFactory_Class final : public UExtensionWidgetFactory
{
	GENERATED_BODY()

public:

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;

protected:

	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
