#pragma once

#include "Input/CommonUIActionRouterBase.h"
#include "UObject/WeakInterfacePtr.h"
#include "RareUIActionRouterBase.generated.h"

class IRareGamepadValuesHandler;
class FRareInputPreprocessor;
class FRareAnalogCursor;

UCLASS(ClassGroup=UI)
class RAREUI_API URareUIActionRouterBase : public UCommonUIActionRouterBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SetAnalogMovementEnabled(const bool bEnable) const;

	void AddGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const;
	void RemoveGamepadValuesListener(const TWeakInterfacePtr<IRareGamepadValuesHandler> InHandler) const;

protected:
	virtual TSharedRef<FCommonAnalogCursor> MakeAnalogCursor() const override;

private: // Internal
	
	TSharedPtr<FRareAnalogCursor> AnalogCursor;
	TSharedPtr<FRareInputPreprocessor> RareInputPreprocessor;
};
