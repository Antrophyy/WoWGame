// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareRichTextBlockActionDecorator.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "RareGameUIManagerSubsystem.h"
#include "RareGameUIPolicy.h"
#include "RarePrimaryGameLayout.h"
#include "RareUISettings.h"
#include "Foundation/RareEnhancedActionWidget.h"
#include "Components/RichTextBlock.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/UIActionBinding.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "RareUI.h"
#include "Foundation/RareActionKeyWidget.h"

FRareRichInlineAction::FRareRichInlineAction(URichTextBlock* InOwner, URareRichTextBlockActionDecorator* InDecorator) : FRichTextDecorator(InOwner)
{
	Decorator = InDecorator;
}

bool FRareRichInlineAction::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return RunParseResult.Name == TEXT("action") && RunParseResult.MetaData.Contains(TEXT("id"));
}

TSharedPtr<SWidget> FRareRichInlineAction::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const
{
	const FString NameRange = RunInfo.MetaData[TEXT("id")];
	const FName Name = FName(NameRange);

	UUserWidget* Outer = Owner->GetTypedOuter<UUserWidget>();

	TSubclassOf<URareActionKeyWidget> ActionKeyWidgetClass;
	if (const URareGameUIManagerSubsystem* GameUIManagerSubsystem = UGameInstance::GetSubsystem<URareGameUIManagerSubsystem>(Outer->GetGameInstance()))
	{
		ActionKeyWidgetClass = GameUIManagerSubsystem->GetCurrentUIPolicy()->GetActionKeyWidget();
	}

	if (!IsValid(ActionKeyWidgetClass))
	{
		ensureMsgf(false, TEXT("URareRichTextBlockActionDecorator::CreateDecoratorWidget: ActionWidgetClass is not valid! Please check your GameUIPolicy configuration."));
		return nullptr;
	}

	// TODO: Should use FUserWidgetPool for this somewhere.
	URareActionKeyWidget* ActionKeyWidget = CreateWidget<URareActionKeyWidget>(Outer, ActionKeyWidgetClass);

	// We don't want to display hold progress bar in rich texts.
	ActionKeyWidget->GetActionWidget()->SetAlwaysHideProgressBar(true);

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	Decorator->RegisterNewDecoratorWidget(Name, ActionKeyWidget);

	return SNew(SBox)
		.HeightOverride(FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f))
		.WidthOverride(FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f))
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			.StretchDirection(EStretchDirection::DownOnly)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				ActionKeyWidget->TakeWidget()
			]
		];
}

TSharedPtr<ITextDecorator> URareRichTextBlockActionDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	const UUserWidget* Outer = InOwner->GetTypedOuter<UUserWidget>();
	ensureMsgf(Outer, TEXT("Shouldn't ever happen, RichTextBlock should always have a UUserWidget parent."));

	OwningLocalPlayer = Outer ? Outer->GetOwningLocalPlayer() : nullptr;
	RegisterListeners();

	return MakeShareable(new FRareRichInlineAction(InOwner, this));
}

void URareRichTextBlockActionDecorator::RegisterNewDecoratorWidget(const FName& InInputActionName, URareActionKeyWidget* InWidget)
{
	FRareInlineActionDecoratorInfo DecoratorInfo;
	DecoratorInfo.InputActionName = InInputActionName;
	DecoratorInfo.ActionKeyWidget = InWidget;

	DecoratorWidgets.Emplace(DecoratorInfo);

	UpdateRepresentedActions();
}

void URareRichTextBlockActionDecorator::RegisterListeners()
{
	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningLocalPlayer.Get()))
	{
		EnhancedInputSubsystem->ControlMappingsRebuiltDelegate.AddUniqueDynamic(this, &ThisClass::UpdateRepresentedActions);
	}

	if (!BoundActionsUpdatedHandle.IsValid())
	{
		if (const UCommonUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(OwningLocalPlayer.Get()))
		{
			BoundActionsUpdatedHandle = ActionRouter->OnBoundActionsUpdated().AddUObject(this, &ThisClass::UpdateRepresentedActions);
		}
	}
}

void URareRichTextBlockActionDecorator::UpdateRepresentedActions()
{
	for (FRareInlineActionDecoratorInfo& DecoratorWidget : DecoratorWidgets)
	{
#if WITH_EDITOR
		const bool bIsDesignTime = !OwningLocalPlayer.IsValid();
		if (bIsDesignTime)
		{
			// We're probably in design time. So let's slap some design time key, so we can see how pretty it looks.
			SetDesignTimeKey(DecoratorWidget);
			continue;
		}

		DecoratorWidget.ActionKeyWidget->SetDesignerFlags(EWidgetDesignFlags::None);
#endif // WITH_EDITOR

		// Let's try to look for Enhanced Input Action first.
		const bool bSuccess = TrySetEnhancedAction(DecoratorWidget);
		if (!bSuccess)
		{
			// We're probably not an Enhanced Input Action then. Let's try to look up UI actions instead.
			TrySetUIAction(DecoratorWidget);
		}
	}
}

bool URareRichTextBlockActionDecorator::TrySetEnhancedAction(const FRareInlineActionDecoratorInfo& DecoratorWidget) const
{
	const UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningLocalPlayer.Get());
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
		if (EnhancedActionMapping.Action.GetName().Equals(DecoratorWidget.InputActionName.ToString()))
		{
			// We are an enhanced input action.
			DecoratorWidget.ActionKeyWidget->GetActionWidget()->SetEnhancedInputAction(const_cast<UInputAction*>(EnhancedActionMapping.Action.Get()));
			return true;
		}
	}

	return false;
}

bool URareRichTextBlockActionDecorator::TrySetUIAction(FRareInlineActionDecoratorInfo& DecoratorWidget) const
{
	const UCommonUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(OwningLocalPlayer.Get());
	if (!IsValid(ActionRouter))
	{
		return false;
	}

	for (const FUIActionBindingHandle& ActiveBindingHandles : ActionRouter->GatherActiveBindings())
	{
		const TSharedPtr<FUIActionBinding> NewActionBinding = FUIActionBinding::FindBinding(ActiveBindingHandles);
		if (!NewActionBinding.IsValid())
			continue;

		const bool bMatchesDecoratorFilter = NewActionBinding->LegacyActionTableRow.RowName.IsEqual(DecoratorWidget.InputActionName);
		if (!bMatchesDecoratorFilter)
			continue;

		// We are UI Action.

		// Let's remove any potential previous action delegate.
		if (DecoratorWidget.CurrentUIBinding.IsValid())
		{
			DecoratorWidget.CurrentUIBinding->OnHoldActionProgressed.Remove(DecoratorWidget.ActionProgressHandle);
		}

		DecoratorWidget.CurrentUIBinding = NewActionBinding;

		// Again start listening for when the hold action progresses.
		DecoratorWidget.ActionProgressHandle = NewActionBinding->OnHoldActionProgressed.AddWeakLambda(this, [DecoratorWidget](const float Progress)
		{
			DecoratorWidget.ActionKeyWidget->GetActionWidget()->OnActionProgress(Progress);
		});

		DecoratorWidget.ActionKeyWidget->GetActionWidget()->SetInputAction(NewActionBinding->LegacyActionTableRow);
		return true;
	}

	return false;
}

#if WITH_EDITOR
void URareRichTextBlockActionDecorator::SetDesignTimeKey(const FRareInlineActionDecoratorInfo& DecoratorWidget) const
{
	DecoratorWidget.ActionKeyWidget->SetDesignerFlags(EWidgetDesignFlags::Designing);
	DecoratorWidget.ActionKeyWidget->GetActionWidget()->SetDesignTimeKey(EKeys::Gamepad_FaceButton_Right);
}
#endif // WITH_EDITOR
