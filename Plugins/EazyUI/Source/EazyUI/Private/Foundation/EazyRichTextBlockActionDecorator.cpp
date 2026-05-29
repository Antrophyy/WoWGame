#include "Foundation/EazyRichTextBlockActionDecorator.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "EazyGameUIManagerSubsystem.h"
#include "EazyGameUIPolicy.h"
#include "EazyPrimaryGameLayout.h"
#include "EazyUISettings.h"
#include "Foundation/EazyEnhancedActionWidget.h"
#include "Components/RichTextBlock.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/UIActionBinding.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "EazyUI.h"
#include "Foundation/EazyActionKeyWidget.h"

FEazyRichInlineAction::FEazyRichInlineAction(URichTextBlock* InOwner, UEazyRichTextBlockActionDecorator* InDecorator) : FRichTextDecorator(InOwner)
{
	Decorator = InDecorator;
}

bool FEazyRichInlineAction::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return RunParseResult.Name == TEXT("action") && RunParseResult.MetaData.Contains(TEXT("id"));
}

TSharedPtr<SWidget> FEazyRichInlineAction::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const
{
	const FString NameRange = RunInfo.MetaData[TEXT("id")];
	const FName Name = FName(NameRange);

	UUserWidget* Outer = Owner->GetTypedOuter<UUserWidget>();
	if (!IsValid(Outer))
	{
		ensureMsgf(false, TEXT("UEazyRichTextBlockActionDecorator::CreateDecoratorWidget: Owner widget outer is invalid."));
		return nullptr;
	}

	TSubclassOf<UEazyActionKeyWidget> ActionKeyWidgetClass;
	if (const UGameInstance* GameInstance = Outer->GetGameInstance())
	{
		if (const UEazyGameUIManagerSubsystem* GameUIManagerSubsystem = UGameInstance::GetSubsystem<UEazyGameUIManagerSubsystem>(GameInstance))
		{
			if (UEazyGameUIPolicy* CurrentPolicy = GameUIManagerSubsystem->GetCurrentUIPolicy())
			{
				ActionKeyWidgetClass = CurrentPolicy->GetActionKeyWidget();
			}
		}
	}

	if (!IsValid(ActionKeyWidgetClass))
	{
		ensureMsgf(false, TEXT("UEazyRichTextBlockActionDecorator::CreateDecoratorWidget: ActionWidgetClass is not valid! Please check your GameUIPolicy configuration."));
		return nullptr;
	}

	// TODO: Should use FUserWidgetPool for this somewhere.
	UEazyActionKeyWidget* ActionKeyWidget = CreateWidget<UEazyActionKeyWidget>(Outer, ActionKeyWidgetClass);

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

TSharedPtr<ITextDecorator> UEazyRichTextBlockActionDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	const UUserWidget* Outer = InOwner->GetTypedOuter<UUserWidget>();
	ensureMsgf(Outer, TEXT("Shouldn't ever happen, RichTextBlock should always have a UUserWidget parent."));

	OwningLocalPlayer = Outer ? Outer->GetOwningLocalPlayer() : nullptr;
	RegisterListeners();

	return MakeShareable(new FEazyRichInlineAction(InOwner, this));
}

void UEazyRichTextBlockActionDecorator::RegisterNewDecoratorWidget(const FName& InInputActionName, UEazyActionKeyWidget* InWidget)
{
	FEazyInlineActionDecoratorInfo DecoratorInfo;
	DecoratorInfo.InputActionName = InInputActionName;
	DecoratorInfo.ActionKeyWidget = InWidget;

	DecoratorWidgets.Emplace(DecoratorInfo);

	UpdateRepresentedActions();
}

void UEazyRichTextBlockActionDecorator::RegisterListeners()
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

void UEazyRichTextBlockActionDecorator::UpdateRepresentedActions()
{
	for (FEazyInlineActionDecoratorInfo& DecoratorWidget : DecoratorWidgets)
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

bool UEazyRichTextBlockActionDecorator::TrySetEnhancedAction(const FEazyInlineActionDecoratorInfo& DecoratorWidget) const
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

bool UEazyRichTextBlockActionDecorator::TrySetUIAction(FEazyInlineActionDecoratorInfo& DecoratorWidget) const
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
void UEazyRichTextBlockActionDecorator::SetDesignTimeKey(const FEazyInlineActionDecoratorInfo& DecoratorWidget) const
{
	DecoratorWidget.ActionKeyWidget->SetDesignerFlags(EWidgetDesignFlags::Designing);
	DecoratorWidget.ActionKeyWidget->GetActionWidget()->SetDesignTimeKey(EKeys::Gamepad_FaceButton_Right);
}
#endif // WITH_EDITOR
