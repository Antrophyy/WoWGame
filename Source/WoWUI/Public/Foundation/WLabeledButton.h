#pragma once

#include "CommonButtonBase.h"
#include "Foundation/EazyButtonBase.h"
#include "WLabeledButton.generated.h"

class UCommonTextBlock;

UCLASS(ClassGroup=UI)
class WOWUI_API UWLabeledButton : public UEazyButtonBase
{
	GENERATED_BODY()

public:
	void SetButtonText(const FText& InText);

protected:
	virtual void NativePreConstruct() override;

protected: // Set by the Editor

	UPROPERTY(EditAnywhere, Category=General, meta=(InlineEditConditionToggle))
	bool bShowButtonLabel = true;

	UPROPERTY(EditAnywhere, Category=General, meta=(EditCondition=bShowButtonLabel))
	FText ButtonText;

private: // Widget Bindings

	UPROPERTY(meta=(BindWidget, AllowPrivateAccess), BlueprintReadOnly)
	TObjectPtr<UCommonTextBlock> ButtonLabel_Text;
};
