#pragma once

#include "UObject/Interface.h"
#include "EazyViewModelWidgetInterface.generated.h"

class UEazyViewModelBase;
class UUserWidget;

UINTERFACE(MinimalAPI)
class UEazyViewModelWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class EAZYUIMVVM_API IEazyViewModelWidgetInterface
{
	GENERATED_BODY()

protected:
	void InitializeViewModelsForWidget(UUserWidget* Widget, TArray<TObjectPtr<UEazyViewModelBase>>& InOutViewModels) const;
	void ReleaseViewModelsForWidget(UUserWidget* Widget, TArray<TObjectPtr<UEazyViewModelBase>>& InOutViewModels) const;
};
