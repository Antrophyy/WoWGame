#pragma once

#include "Foundation/WcUserWidget.h"
#include "WcItemTooltipWidget.generated.h"

struct FWcItemBase;
class UWcItemBase;
class UWcTextBlock;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWcItemTooltipWidget : public UWcUserWidget
{
	GENERATED_BODY()

public:

	void InitializeTooltipInformation(const UWcItemBase* Item) const;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcTextBlock> Name_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcTextBlock> BindType_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcTextBlock> Type_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWcTextBlock> LevelRequirement_Label;
};
