#pragma once

#include "WIndicatorHandler_Base.h"
#include "UObject/ObjectKey.h"
#include "WIndicatorHandler_Nameplate.generated.h"

class UGameIndicatorDescriptor;
class UIndicatorDescriptor;

UCLASS(ClassGroup=UI)
class WOWUI_API UWIndicatorHandler_Nameplate final : public UWIndicatorHandler_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeComponentInitialized() override;

private: // Internal

	UFUNCTION()
	void HandleNameplateVisibilityBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleNameplateVisibilityEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	TMap<FString, TObjectKey<UGameIndicatorDescriptor>> IndicatorOwnersMap;
};
