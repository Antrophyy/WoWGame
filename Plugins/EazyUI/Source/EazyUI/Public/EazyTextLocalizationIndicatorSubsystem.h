#pragma once

#include "Delegates/Delegate.h"
#include "Subsystems/EngineSubsystem.h"
#include "EazyTextLocalizationIndicatorSubsystem.generated.h"

class FSubsystemCollectionBase;

// Subsystem to manage the EazyText localization indicator debug drawing.
UCLASS()
class EAZYUI_API UEazyTextLocalizationIndicatorSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	FDelegateHandle DebugDrawDelegate;
};

