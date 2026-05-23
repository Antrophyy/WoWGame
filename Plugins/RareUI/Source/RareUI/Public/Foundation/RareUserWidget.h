// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "Blueprint/UserWidget.h"
#include "RareUserWidget.generated.h"

UCLASS(Abstract, ClassGroup=UI)
class RAREUI_API URareUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// Validate properties that should be set in the editor. Loops through all properties with tag "validate" or "validateIgnoreParent" and checks if they are set.
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif // WITH_EDITOR

protected:
	virtual void NativeConstruct() override;
	
protected: // RareUI Overridable Functions
	
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
};
