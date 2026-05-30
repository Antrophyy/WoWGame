#pragma once

#include "Blueprint/UserWidget.h"
#include "EazyViewModelWidgetInterface.h"
#include "EazyUserWidget.generated.h"

class UEazyViewModelBase;

UCLASS(Abstract, ClassGroup=UI)
class EAZYUI_API UEazyUserWidget : public UUserWidget, public IEazyViewModelWidgetInterface
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// Validate properties that should be set in the editor. Loops through all properties with tag "validate" or "validateIgnoreParent" and checks if they are set.
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected: // EazyUI Overridable Functions
	
	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	UFUNCTION(BlueprintImplementableEvent, Category= AnimationStates, meta = (DisplayName = "Reset Widget Animations State"))
	void BP_ResetWidgetAnimationsState();
	
	// Called on construct. Does nothing on its own. Override to reset animations state manually per property that could have been animated.
	virtual void NativeResetWidgetAnimationsState();

protected:
	
	// Gets the HUD associated with Player Controller.
	AHUD* GetHUD() const;

	// Gets the HUD associated with Player Controller. cast to the template type.
	template <class T>
	T* GetHUD() const
	{
		return Cast<T>(GetHUD());
	}

private:
	UPROPERTY()
	TArray<TObjectPtr<UEazyViewModelBase>> ViewModels;

	UPROPERTY()
	TArray<int32> ViewModelSourceIndices;
};
