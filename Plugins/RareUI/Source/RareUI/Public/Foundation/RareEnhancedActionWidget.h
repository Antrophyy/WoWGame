#pragma once

#include "CommonActionWidget.h"
#include "RareEnhancedActionWidget.generated.h"

struct FInputActionInstance;
class UInputAction;
class UMaterialInstanceDynamic;
class STextBlock;

DECLARE_MULTICAST_DELEGATE(FOnActionCompleteDelegate);

/**
 * A widget that shows a platform-specific icon for the given input action. Can use either EI or Legacy input without needing EI settings to be enabled
 * If legacy option is filled, it will take priority over EI
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=UI)
class RAREUI_API URareEnhancedActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()

public:
	
	/** Begin UWidget */
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual FSlateBrush GetIcon() const override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;
	/** End UWidget */

	/** Begin UCommonActionWidget */
	virtual bool IsHeldAction() const override;
	/** End UCommonActionWidget */
	
	virtual FText GetActionDisplayText() const;
	
	void SetAlwaysHideProgressBar(const bool bHide);

	// Sets whether the progress display of the action should update (fill up).
	void SetIsInteractionEnabled(const bool bInInteractionEnabled);

	// Gets whether the progress display of the action should update (fill up).
	bool IsInteractionEnabled() const;
	
	void SetEnhancedInputAction(UInputAction* InInputAction, const int32 KeyIndex);
	virtual void SetEnhancedInputAction(UInputAction* InInputAction) override;
	
#if WITH_EDITOR
	void SetDesignTimeKey(const FKey& InDesignTimeKey);
#endif // WITH_EDITOR

	/**
	 * The Gamepad material to use when showing held progress, the progress will be sent using the material parameter
	 * defined by ProgressMaterialParam and the value will range from 0..1.
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RareActionWidget)
	FSlateBrush GamepadProgressMaterialBrush;

	/**
	 * The Mouse & Keyboard material to use when showing held progress, the progress will be sent using the material parameter
	 * defined by ProgressMaterialParam and the value will range from 0..1.
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RareActionWidget)
	FSlateBrush KeyboardProgressMaterialBrush;

	/**
	 * Size override for the icon. If set to 0, the default size for that dimension will be used.
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RareActionWidget)
	FVector2D IconSizeOverride;

protected:

	/** Begin UCommonActionWidget */
	virtual void UpdateActionWidget() override;
	virtual bool ShouldUpdateActionWidgetIcon() const override;
	virtual void OnWidgetRebuilt() override;
	/** End UCommonActionWidget */

	void OnEnhancedActionHeld(const FInputActionInstance& Instance);
	void OnEnhancedActionReset(const FInputActionInstance& Instance);
	void OnEnhancedActionCompleted(const FInputActionInstance& Instance);

	void RegisterEnhancedInputControlMappingRebuilt();

	bool InputActionContainsHoldTrigger() const;
	bool InputMappingContextContainsHoldTrigger() const;

	virtual void ReplaceIconWithActionDisplayName();
	
	UFUNCTION()
	void HandleControlMappingsRebuilt();

	UPROPERTY(EditAnywhere, Category=CommonActionWidget)
	bool bUseDisplayNameAsFallback = false;
	
	UPROPERTY(EditAnywhere, Category=CommonActionWidget, meta=(EditCondition="bUseDisplayNameAsFallback"))
	FTextBlockStyle KeyboardActionNameFont;

	UPROPERTY(EditAnywhere, Category=CommonActionWidget, meta=(EditCondition="bUseDisplayNameAsFallback"))
	FSlateBrush KeyboardActionBackgroundBrush;

	UPROPERTY(EditAnywhere, Category=CommonActionWidget)
	TObjectPtr<const UInputAction> RepresentedEnhancedInputAction;
	
	UPROPERTY(EditAnywhere, Category=CommonActionWidget)
	bool bAlwaysHideProgressBar = false;
	
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> GamepadProgressDynamicMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> KeyboardProgressDynamicMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> CachedProgressDynamicMaterial;

	FSlateBrush DisplayedProgressBrush;

	TSharedPtr<STextBlock> MyActionText;

private: // Internal
	
	void UpdateProgressMaterialStyle();
	void BuildMaterial(TObjectPtr<UMaterialInstanceDynamic>& InProgressDynamicMaterial, FSlateBrush& ProgressSlateBrush);

	uint32 EnhancedActionProgressBindingHandle = 0;
	uint32 EnhancedActionCanceledBindingHandle = 0;
	uint32 EnhancedActionCompletedBindingHandle = 0;

	int32 ActionKeyIndex = 0;

	bool bInteractionEnabled = true;

	mutable float EnhancedHoldActionHoldTime = 0.f;
};
