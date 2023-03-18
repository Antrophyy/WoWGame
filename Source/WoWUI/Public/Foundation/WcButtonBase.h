#pragma once

#include "CommonButtonBase.h"
#include "WcButtonBase.generated.h"

class UCommonTextBlock;

UCLASS(ClassGroup=UI)
class WOWUI_API UWcButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public: // Exposed functions

	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);
	
public: // Button Properties

	UPROPERTY(Category=Button, EditAnywhere, meta=(InlineEditConditionToggle))
	bool bShowButtonLabel = true;

	UPROPERTY(Category=Button, EditAnywhere, meta=(EditCondition=bShowButtonLabel))
	FText ButtonText = FText::FromString(TEXT("{BUTTON_TEXT}"));

protected: // Overriden Functions

	virtual void NativePreConstruct() override;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ButtonText_Label;
};
