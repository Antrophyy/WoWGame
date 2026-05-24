// Copyright (C) Grip Studios. All Rights Reserved

#include "Localization/RareTextLocalizationIndicatorSubsystem.h"

#include "CanvasItem.h"
#include "CommonNumericTextBlock.h"
#include "ShowFlags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/SWindow.h"

namespace RareTextLocalizationIndicatorSubsystem
{
	constexpr const TCHAR* RareTextLocalizationIndicatorName = TEXT("RareTextLocalizationIndicator");
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> RareTextLocalizationIndicatorShowFlag(
		RareTextLocalizationIndicatorName,
		false);

	static bool ValidateText(const FText& Text, FString& OutReason)
	{
		if (Text.IsFromStringTable())
		{
			FName TableId;
			FTextKey Key;
			FTextInspector::GetTableIdAndKey(Text, TableId, Key);
			OutReason = FString::Printf(TEXT("[%s, %s]"), *TableId.ToString(), *Key.ToString());
			return true;
		}

		if (Text.IsCultureInvariant())
		{
			OutReason = TEXT("Culture Invariant");
			return false;
		}

		TOptional<FString> Namespace = FTextInspector::GetNamespace(Text);
		if (!Namespace.IsSet())
		{
			OutReason = TEXT("Missing namespace");
			return false;
		}

		const FString& NamespaceStr = Namespace.GetValue();
		
		const bool bIsBracketedNamespace = NamespaceStr.StartsWith(TEXT("[")) && NamespaceStr.EndsWith(TEXT("]"));
		
		if (NamespaceStr.IsEmpty() || bIsBracketedNamespace)
		{
			// Namespace only has a GUID set, we want to enforce a namespace to be always set
			OutReason = TEXT("Missing namespace");
			return false;
		}

		const TOptional<FString> Key = FTextInspector::GetKey(Text);
		OutReason = FString::Printf(TEXT("[%s, %s]"), *NamespaceStr, *Key.Get(TEXT("<unset>")));
		return true;
	}

	static void RareTextLocalizationIndicatorDraw(UCanvas* Canvas, APlayerController* Controller)
	{
		for (TObjectIterator<UTextBlock> It; It; ++It)
		{
			if (Cast<UCommonNumericTextBlock>(*It))
			{
				// Filter out numeric textblocks.
				continue;
			}
			
			UTextBlock* TextBlock = *It;
			if (!TextBlock)
				continue;

			if (TextBlock->IsDesignTime())
				continue;

			TSharedPtr<SWidget> Widget = TextBlock->GetCachedWidget();
			if (!Widget.IsValid())
				continue;

			bool bIsVisible = true;
			FGeometry ViewportGeometry;
			
			{
				TSharedPtr<SWidget> Parent = Widget;
				while (Parent.IsValid())
				{
					EVisibility Visibility = Parent->GetVisibility();
					if (Parent->GetRenderOpacity() == 0.0f || Visibility == EVisibility::Collapsed || Visibility == EVisibility::Hidden)
					{
						bIsVisible = false;
						break;
					}
					
					if (Parent->Advanced_IsWindow())
					{
						TSharedPtr<SWindow> Window = StaticCastSharedPtr<SWindow>(Parent);
						bIsVisible = Window->IsVisible() && Window->GetViewport().IsValid();
						if (bIsVisible)
						{
							TSharedPtr<SWidget> ViewportWidget = Window->GetViewport()->GetWidget().Pin();
							check(ViewportWidget.IsValid());
							ViewportGeometry = ViewportWidget->GetCachedGeometry();
						}
						break;
					}
					Parent = Parent->GetParentWidget();
				}
			}

			if (!bIsVisible)
			{
				continue;
			}

			FString Text;
			bool bIsTextValid = ValidateText(TextBlock->GetText(), Text);
			FColor Color = bIsTextValid ? FColor::Green : FColor::Red;

			FGeometry Geometry = Widget->GetCachedGeometry();
			
			TArray<APlayerController*> PlayerControllers;
			GEngine->GetAllLocalPlayerControllers(PlayerControllers);

			FVector2D ViewportPosition = ViewportGeometry.AbsoluteToLocal(Geometry.GetAbsolutePosition());
			FVector2D ViewportSize = Geometry.GetAbsoluteSize() * UWidgetLayoutLibrary::GetViewportScale(PlayerControllers[0]);

			FCanvasBoxItem Item(ViewportPosition, ViewportSize);
			Item.SetColor(Color);
			Item.LineThickness = 1.0f;
			Canvas->DrawItem(Item);

			float TextX, TextY;
			Canvas->TextSize(UEngine::GetSmallFont(), Text, TextX, TextY);
			FVector2D TextSize = {TextX, TextY};

			FVector2D TextPosition = ViewportPosition + ViewportSize - TextSize;
			FFontRenderInfo RenderInfo = Canvas->CreateFontRenderInfo(false, true);
			Canvas->SetDrawColor(Color);
			Canvas->DrawText(UEngine::GetSmallFont(), Text, TextPosition.X - 2.f, TextPosition.Y - 2.f, 1, 1, RenderInfo);
		}
	}
}

void URareTextLocalizationIndicatorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	using namespace RareTextLocalizationIndicatorSubsystem;
	DebugDrawDelegate = UDebugDrawService::Register(RareTextLocalizationIndicatorName, FDebugDrawDelegate::CreateStatic(RareTextLocalizationIndicatorDraw));
}

void URareTextLocalizationIndicatorSubsystem::Deinitialize()
{
	UDebugDrawService::Unregister(DebugDrawDelegate);
}
