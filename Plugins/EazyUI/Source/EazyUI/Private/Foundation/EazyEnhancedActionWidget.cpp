#include "Foundation/EazyEnhancedActionWidget.h"

#include "CommonInputSubsystem.h"
#include "CommonUIExtensions.h"
#include "CommonUITypes.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LogEazyUI.h"
#include "EazyUI.h"
#include "Input/UIActionBinding.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "EazyWidgetPaletteCategories.h"
#include "Engine/LocalPlayer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<SWidget> UEazyEnhancedActionWidget::RebuildWidget()
{
	BuildMaterial(GamepadProgressDynamicMaterial, GamepadProgressMaterialBrush);
	BuildMaterial(KeyboardProgressDynamicMaterial, KeyboardProgressMaterialBrush);
	BuildMaterial(CachedProgressDynamicMaterial, ProgressMaterialBrush);

	bInteractionEnabled = GetIsEnabled();
	
	// Set progress to 0 by default
	if (IsValid(ProgressDynamicMaterial) && !ProgressMaterialParam.IsNone())
	{
		ProgressDynamicMaterial->SetScalarParameterValue(ProgressMaterialParam, 0);
	}

	MyKeyBox = SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::SelfHitTestInvisible);

	MyKeyBox->SetContent(
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(MyIconRim, SImage)
			.Image(&IconRimBrush)
			.Visibility(EVisibility::SelfHitTestInvisible)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(MyIcon, SImage)
			.Image(&Icon)
			.Visibility(EVisibility::SelfHitTestInvisible)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(-8)
		[
			SAssignNew(MyProgressImage, SImage)
			.Image(&DisplayedProgressBrush)
			.Visibility(EVisibility::SelfHitTestInvisible)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SScaleBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Stretch(EStretch::ScaleToFit)
			.Visibility(EVisibility::SelfHitTestInvisible)
			.StretchDirection(EStretchDirection::Both)
			[
				SAssignNew(MyActionText, STextBlock)
				.Justification(ETextJustify::Center)
				.TextStyle(&KeyboardActionNameFont)
				.Visibility(EVisibility::Collapsed)
			]
		]);

	return MyKeyBox.ToSharedRef();
}

void UEazyEnhancedActionWidget::ReleaseSlateResources(const bool bReleaseChildren)
{
	if (IsValid(RepresentedEnhancedInputAction) && IsValid(GetOwningLocalPlayer()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = GetOwningLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnhancedInputSubsystem->ControlMappingsRebuiltDelegate.RemoveAll(this);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetOwningPlayer()->InputComponent))
		{
			EnhancedInputComponent->RemoveBindingByHandle(EnhancedActionProgressBindingHandle);
			EnhancedInputComponent->RemoveBindingByHandle(EnhancedActionCanceledBindingHandle);
			EnhancedInputComponent->RemoveBindingByHandle(EnhancedActionCompletedBindingHandle);
		}
	}
	
	Super::ReleaseSlateResources(bReleaseChildren);
}

FSlateBrush UEazyEnhancedActionWidget::GetIcon() const
{
	if (!IsDesignTime())
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem())
		{
			return IsValid(RepresentedEnhancedInputAction)
				       ? UCommonUIExtensions::GetIconForInputAction(CommonInputSubsystem, RepresentedEnhancedInputAction, ActionKeyIndex)
				       : CommonUI::GetIconForInputActions(CommonInputSubsystem, InputActions);
		}
	}
#if WITH_EDITORONLY_DATA
	else
	{
		if (DesignTimeKey.IsValid())
		{
			ECommonInputType Dummy;
			FName OutDefaultGamepadName;
			FCommonInputBase::GetCurrentPlatformDefaults(Dummy, OutDefaultGamepadName);

			ECommonInputType KeyInputType = ECommonInputType::MouseAndKeyboard;
			if (DesignTimeKey.IsGamepadKey())
			{
				KeyInputType = ECommonInputType::Gamepad;
			}
			else if (DesignTimeKey.IsTouch())
			{
				KeyInputType = ECommonInputType::Touch;
			}

			FSlateBrush InputBrush;
			if (UCommonInputPlatformSettings::Get()->TryGetInputBrush(InputBrush, TArray<FKey>{DesignTimeKey}, KeyInputType, OutDefaultGamepadName))
			{
				return InputBrush;
			}
		}
	}
#endif

	return *FStyleDefaults::GetNoBrush();
}

FText UEazyEnhancedActionWidget::GetActionDisplayText() const
{
	if (IsValid(RepresentedEnhancedInputAction))
	{
		return RepresentedEnhancedInputAction->ActionDescription;
	}

	const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	if (GetGameInstance() && CommonInputSubsystem)
	{
		if (const FCommonInputActionDataBase* InputActionData = GetInputActionData())
		{
			const FCommonInputTypeInfo& InputTypeInfo = InputActionData->GetCurrentInputTypeInfo(CommonInputSubsystem);

			if (InputTypeInfo.bActionRequiresHold)
			{
				return InputActionData->HoldDisplayName;
			}
			
			return InputActionData->DisplayName;
		}
	}
	return FText();
}

void UEazyEnhancedActionWidget::SetAlwaysHideProgressBar(const bool bHide)
{
	bAlwaysHideProgressBar = bHide;
	UpdateActionWidget();
}

void UEazyEnhancedActionWidget::SetIsInteractionEnabled(const bool bInInteractionEnabled)
{
	bInteractionEnabled = bInInteractionEnabled;
}

bool UEazyEnhancedActionWidget::IsInteractionEnabled() const
{
	return bInteractionEnabled;
}

bool UEazyEnhancedActionWidget::IsHeldAction() const
{
	if (IsValid(RepresentedEnhancedInputAction))
	{
		return InputActionContainsHoldTrigger() || InputMappingContextContainsHoldTrigger();
	}
	
	const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	if (GetGameInstance() && CommonInputSubsystem)
	{
		if (const FCommonInputActionDataBase* InputActionData = GetInputActionData())
		{
			const FCommonInputTypeInfo& InputTypeInfo = InputActionData->GetCurrentInputTypeInfo(CommonInputSubsystem);

			return InputTypeInfo.bActionRequiresHold;
		}
	}

	return false;
}

void UEazyEnhancedActionWidget::SetEnhancedInputAction(UInputAction* InInputAction, const int32 KeyIndex)
{
	ActionKeyIndex = KeyIndex;
	SetEnhancedInputAction(InInputAction);
}

void UEazyEnhancedActionWidget::SetEnhancedInputAction(UInputAction* InInputAction)
{
	if (IsDesignTime())
	{
		return;
	}

	UpdateBindingHandleInternal(FUIActionBindingHandle());
	RepresentedEnhancedInputAction = InInputAction;
	InputActions.Reset();

	RegisterEnhancedInputControlMappingRebuilt();
	
	if (IsValid(RepresentedEnhancedInputAction))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetOwningPlayer()->InputComponent))
		{
			if (IsHeldAction())
			{
				EnhancedActionProgressBindingHandle = EnhancedInputComponent->BindAction(RepresentedEnhancedInputAction, ETriggerEvent::Ongoing, this, &ThisClass::OnEnhancedActionHeld).GetHandle();
				EnhancedActionCanceledBindingHandle = EnhancedInputComponent->BindAction(RepresentedEnhancedInputAction, ETriggerEvent::Canceled, this, &ThisClass::OnEnhancedActionReset).GetHandle();
				EnhancedActionCompletedBindingHandle = EnhancedInputComponent->BindAction(RepresentedEnhancedInputAction, ETriggerEvent::Completed, this, &ThisClass::OnEnhancedActionCompleted).GetHandle();
			}
			else
			{
				EnhancedInputComponent->RemoveBindingByHandle(EnhancedActionProgressBindingHandle);
				EnhancedInputComponent->RemoveBindingByHandle(EnhancedActionCanceledBindingHandle);
				EnhancedInputComponent->RemoveBindingByHandle(EnhancedActionCompletedBindingHandle);
			}
		}
	}
	
	UpdateActionWidget();
}

#if WITH_EDITOR
void UEazyEnhancedActionWidget::SetDesignTimeKey(const FKey& InDesignTimeKey)
{
	if (IsDesignTime())
	{
#if WITH_EDITORONLY_DATA
		DesignTimeKey = InDesignTimeKey;
#endif // WITH_EDITORONLY_DATA
		UpdateActionWidget();
	}
}
#endif // WITH_EDITOR

void UEazyEnhancedActionWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	RegisterEnhancedInputControlMappingRebuilt();
}

void UEazyEnhancedActionWidget::OnEnhancedActionHeld(const FInputActionInstance& Instance)
{
	if (!bInteractionEnabled)
	{
		return;
	}
	
	if (FMath::IsNearlyEqual(EnhancedHoldActionHoldTime, 0))
	{
		ensureMsgf(false, TEXT("UEazyEnhancedActionWidget::OnEnhancedActionHeld called with EnhancedHoldActionHoldTime > 0. This should not happen, as the hold time should be set to 0 when the action is held. Please check your input action configuration."));
		return;
	}
	
	OnActionProgress(Instance.GetElapsedTime() / EnhancedHoldActionHoldTime);
}

void UEazyEnhancedActionWidget::OnEnhancedActionReset(const FInputActionInstance&)
{
	OnActionComplete();
}

void UEazyEnhancedActionWidget::OnEnhancedActionCompleted(const FInputActionInstance&)
{
	OnActionComplete();
}

void UEazyEnhancedActionWidget::RegisterEnhancedInputControlMappingRebuilt()
{
	// We're relying on Enhanced Input actions, so we need to listen for when the control mappings are rebuilt. Keys may not be ready when we initialize.
	if (IsValid(RepresentedEnhancedInputAction) && IsValid(GetOwningLocalPlayer()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = GetOwningLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnhancedInputSubsystem->ControlMappingsRebuiltDelegate.AddUniqueDynamic(this, &ThisClass::HandleControlMappingsRebuilt);
		}
	}
}

bool UEazyEnhancedActionWidget::InputActionContainsHoldTrigger() const
{
	for (const TObjectPtr<UInputTrigger>& Trigger : RepresentedEnhancedInputAction->Triggers)
	{
		const UInputTriggerHold* TriggerHold = Cast<UInputTriggerHold>(Trigger);
		if (IsValid(TriggerHold))
		{
			EnhancedHoldActionHoldTime = TriggerHold->HoldTimeThreshold;
			return true;
		}
	}

	return false;
}

bool UEazyEnhancedActionWidget::InputMappingContextContainsHoldTrigger() const
{
	const UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer());
	if (!IsValid(System))
	{
		return false;
	}

	UEnhancedPlayerInput* PlayerInput = System->GetPlayerInput();
	if (!IsValid(PlayerInput))
	{
		return false;
	}

	for (const FEnhancedActionKeyMapping& EnhancedActionMapping : GetEnhancedActionKeyMappingsOf(PlayerInput))
	{
		if (EnhancedActionMapping.Action == RepresentedEnhancedInputAction)
		{
			for (const TObjectPtr<UInputTrigger>& Trigger : EnhancedActionMapping.Triggers)
			{
				const UInputTriggerHold* TriggerHold = Cast<UInputTriggerHold>(Trigger);
				if (IsValid(TriggerHold))
				{
					EnhancedHoldActionHoldTime = TriggerHold->HoldTimeThreshold;
					return true;
				}
			}

			break;
		}
	}

	return false;
}

void UEazyEnhancedActionWidget::ReplaceIconWithActionDisplayName()
{
	const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	if (!IsValid(CommonInputSubsystem))
	{
		return;
	}

	MyActionText->SetVisibility(EVisibility::SelfHitTestInvisible);
	
	FKey Key;

	bool bFoundInputAction = false;
	const bool bIsEnhancedInputAction = IsValid(RepresentedEnhancedInputAction);
	if (bIsEnhancedInputAction)
	{
		Key = UCommonUIExtensions::GetKeyForInputType(GetOwningLocalPlayer(), CommonInputSubsystem->GetCurrentInputType(), RepresentedEnhancedInputAction, ActionKeyIndex);

		bFoundInputAction = true;
	}
	else
	{
		const FCommonInputActionDataBase* InputActionData = GetInputActionData();
		if (InputActionData)
		{
			const FCommonInputTypeInfo& TypeInfo = InputActionData->GetCurrentInputTypeInfo(CommonInputSubsystem);
			Key = TypeInfo.GetKey();
			
			bFoundInputAction = true;
		}
	}

	if (!bFoundInputAction)
	{
		ensureMsgf(false, TEXT("This should not ever happen. We should always have either an Enhanced Input Action or a Common Input Action Data assigned to this widget."));
		FDebug::DumpStackTraceToLog(TEXT("IconDisplayBadForKeyHint"), ELogVerbosity::Error);
	}
	
	Icon = KeyboardActionBackgroundBrush;

	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard)
	{
		// For PC, let's show the actual key name as fallback.
		MyActionText->SetText(Key.GetDisplayName());
	}
	else
	{
		// For gamepad, since the names would be way too long, let's just show a "?" instead with an error message.
		MyActionText->SetText(FText::FromString("?"));
	}
	
	MyActionText->SetVisibility(EVisibility::SelfHitTestInvisible);
}

void UEazyEnhancedActionWidget::UpdateActionWidget()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	UpdateProgressMaterialStyle();
	
	bool bDirty = false;
	
	const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	if (IsDesignTime() || (GetGameInstance() && ensure(CommonInputSubsystem) && CommonInputSubsystem->ShouldShowInputKeys()))
	{
		if (ShouldUpdateActionWidgetIcon())
		{
			const FSlateBrush PreviousIcon = Icon;
			Icon = GetIcon();

			// Check if the icon changed
			bDirty = PreviousIcon != Icon;
			
			if (Icon.DrawAs == ESlateBrushDrawType::NoDrawType)
			{
				// Seems the icon doesn't exist, let's show a display name of the action instead.
				if (bUseDisplayNameAsFallback && MyActionText.IsValid())
				{
					const FText& PreviousText = MyActionText->GetText();
					ReplaceIconWithActionDisplayName();
					const FText& NewText = MyActionText->GetText();

					if (!PreviousText.EqualTo(NewText))
					{
						bDirty = true;
					}
				}
				else
				{
					if (!IsDesignTime())
					{
						SetVisibility(ESlateVisibility::Collapsed);
					}
				}
			}
			else
			{
				if (MyActionText.IsValid())
				{
					MyActionText->SetVisibility(EVisibility::Collapsed);
				}
			}

			if (MyIcon.IsValid())
			{
				MyIcon->SetImage(&Icon);

				if (IconSizeOverride.X > 0)
				{
					MyKeyBox->SetWidthOverride(IconSizeOverride.X);
				}

				if (IconSizeOverride.Y > 0)
				{
					MyKeyBox->SetHeightOverride(IconSizeOverride.Y);
				}
				
				OnInputIconUpdated.Broadcast();

				if (GetVisibility() != ESlateVisibility::Collapsed)
				{
					// The object being passed into SetImage is the same each time so layout is never invalidated
					// Manually invalidate it here as the dimensions may have changed

					if (bDirty)
					{
						MyIcon->Invalidate(EInvalidateWidgetReason::Layout);
					}
				}

				if (MyProgressImage.IsValid())
				{
					if (IsHeldAction() && !bAlwaysHideProgressBar)
					{
						if (MyProgressImage->GetVisibility() != EVisibility::SelfHitTestInvisible)
						{
							bDirty = true;
							MyProgressImage->SetVisibility(EVisibility::SelfHitTestInvisible);
						}
					}
					else
					{
						if (MyProgressImage->GetVisibility() != EVisibility::Collapsed)
						{
							bDirty = true;
							MyProgressImage->SetVisibility(EVisibility::Collapsed);
						}
					}
				}

				if (MyKeyBox.IsValid() && bDirty)
				{
					MyKeyBox->Invalidate(EInvalidateWidget::Layout);
				}

				if (!IsDesignTime())
				{
					SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}

				return;
			}
		}
		if (!IsDesignTime())
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

bool UEazyEnhancedActionWidget::ShouldUpdateActionWidgetIcon() const
{
	if (bAlwaysHideOverride)
	{
		return false;
	}

	const FCommonInputActionDataBase* InputActionData = GetInputActionData();

#if WITH_EDITORONLY_DATA
	const bool bIsDesignPreview = IsDesignTime() && DesignTimeKey.IsValid();
#else
	const bool bIsDesignPreview = false;
#endif

	return InputActionData || IsValid(RepresentedEnhancedInputAction) || bIsDesignPreview;
}

void UEazyEnhancedActionWidget::HandleControlMappingsRebuilt()
{
	UpdateActionWidget();
}

void UEazyEnhancedActionWidget::UpdateProgressMaterialStyle()
{
	if (!IsDesignTime())
	{
		if (const UCommonInputSubsystem* InputSubsystem = GetInputSubsystem())
		{
			// Set displayed material based on input method
			if (InputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad && GamepadProgressMaterialBrush.IsSet() && GamepadProgressDynamicMaterial)
			{
				DisplayedProgressBrush = GamepadProgressMaterialBrush;
				ProgressDynamicMaterial = GamepadProgressDynamicMaterial;
			}
			else if (InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard && KeyboardProgressMaterialBrush.IsSet() && KeyboardProgressDynamicMaterial)
			{
				DisplayedProgressBrush = KeyboardProgressMaterialBrush;
				ProgressDynamicMaterial = KeyboardProgressDynamicMaterial;
			}
			else if (ProgressMaterialBrush.IsSet() && CachedProgressDynamicMaterial)
			{
				DisplayedProgressBrush = ProgressMaterialBrush;
				ProgressDynamicMaterial = CachedProgressDynamicMaterial;
			}
		}
	}
}

void UEazyEnhancedActionWidget::BuildMaterial(TObjectPtr<UMaterialInstanceDynamic>& InProgressDynamicMaterial, FSlateBrush& ProgressSlateBrush)
{
	if (!IsDesignTime() && !InProgressDynamicMaterial)
	{
		if (!IconSizeOverride.IsZero())
		{
			ProgressSlateBrush.SetImageSize(IconSizeOverride);
		}
		
		UMaterialInstanceDynamic* const ParentMaterialDynamic = Cast<UMaterialInstanceDynamic>(ProgressSlateBrush.GetResourceObject());
		if (ParentMaterialDynamic == nullptr)
		{
			UMaterialInterface* ParentMaterial = Cast<UMaterialInterface>(ProgressSlateBrush.GetResourceObject());
			if (ParentMaterial)
			{
				InProgressDynamicMaterial = UMaterialInstanceDynamic::Create(ParentMaterial, this);
				ProgressSlateBrush.SetResourceObject(InProgressDynamicMaterial);
			}
			else
			{
				InProgressDynamicMaterial = nullptr;
			}
		}
	}
}
