#pragma once

#include "Foundation/WUserWidget.h"
#include "WItemTooltipWidget.generated.h"

class UTextBlock;
struct FWcItemDetail;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWItemTooltipWidget : public UWUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTooltipInformation(const FWcItemDetail& Item) const;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Name_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BindType_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Type_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> LevelRequirement_Label;
};
