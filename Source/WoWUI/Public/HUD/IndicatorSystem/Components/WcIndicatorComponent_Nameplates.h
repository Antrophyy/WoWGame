#pragma once

#include "WcIndicatorComponent_Base.h"
#include "WcIndicatorComponent_Nameplates.generated.h"

class UIndicatorDescriptor;

UCLASS()
class WOWUI_API UWcIndicatorComponent_Nameplates final : public UWcIndicatorComponent_Base
{
	GENERATED_BODY()

protected:

	virtual void NativeComponentInitialized() override;

private: // Delegate Handling

	UFUNCTION()
	void HandleNameplateVisibilityBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleNameplateVisibilityEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	UPROPERTY()
	TMap<FString, TObjectPtr<UIndicatorDescriptor>> IndicatorOwnersMap;
};
