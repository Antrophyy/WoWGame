
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "RareTextLocalizationIndicatorSubsystem.generated.h"

/**
 * Subsystem to manage the RareText localization indicator debug drawing.
 */
UCLASS()
class RAREUI_API URareTextLocalizationIndicatorSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	FDelegateHandle DebugDrawDelegate;
};
