#pragma once

#include "Components/RichTextBlockDecorator.h"

#include "RareRichTextBlockActionDecorator.generated.h"

struct FUIActionBinding;
class URareRichTextBlockActionDecorator;
class ULocalPlayer;
class URareEnhancedActionWidget;

class FRareRichInlineAction final : public FRichTextDecorator
{
public:
	FRareRichInlineAction(URichTextBlock* InOwner, URareRichTextBlockActionDecorator* InDecorator);

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override;

private:
	TWeakObjectPtr<URareRichTextBlockActionDecorator> Decorator;
};

USTRUCT(Category=UI)
struct FRareInlineActionDecoratorInfo
{
	GENERATED_BODY()

public: // General
	FName InputActionName;

	UPROPERTY()
	TObjectPtr<URareEnhancedActionWidget> ActionWidget;

public: // UI Action specifics
	
	TSharedPtr<FUIActionBinding> CurrentUIBinding;
	FDelegateHandle ActionProgressHandle;
};

/**
 * This decorator is used to create a widget for a specific input action. Works for both UI actions and enhanced input actions (RowName in datatable).
 * Understands the format <action id="NameOfInputAction"/>, so, for "IA_Jump", the name would be "IA_Jump". If it's UI action, it should show Row Name in the Data Table.
 * Multiple actions can be used in a single RichTextBlock.
 */
UCLASS(Blueprintable, ClassGroup=UI)
class URareRichTextBlockActionDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
	void RegisterNewDecoratorWidget(const FName& InInputActionName, URareEnhancedActionWidget* InWidget);
	
private:
	void RegisterListeners();

	bool TrySetEnhancedAction(const FRareInlineActionDecoratorInfo& DecoratorWidget) const;
	bool TrySetUIAction(FRareInlineActionDecoratorInfo& DecoratorWidget) const;

#if WITH_EDITOR
	void SetDesignTimeKey(const FRareInlineActionDecoratorInfo& DecoratorWidget) const;
#endif // WITH_EDITOR
	
	UFUNCTION()
	void UpdateRepresentedActions();

	TWeakObjectPtr<ULocalPlayer> OwningLocalPlayer;
	FDelegateHandle BoundActionsUpdatedHandle;
	
	UPROPERTY(Transient)
	TArray<FRareInlineActionDecoratorInfo> DecoratorWidgets;
};