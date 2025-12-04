#pragma once

#include "RareWidgetFactory.h"
#include "RareWidgetFactory_Class.generated.h"

UCLASS(ClassGroup=UI)
class RAREUI_API URareWidgetFactory_Class final : public URareWidgetFactory
{
	GENERATED_BODY()

public:

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;

protected:

	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
