#pragma once

#include "Engine/Texture2D.h"
#include "Foundation/WcUserWidget.h"
#include "Items/WcItemDetail.h"
#include "WcItemWidget.generated.h"

class UImage;
struct FWcItemDetail;

UCLASS(ClassGroup=UI)
class WOWUI_API UWcItemWidget : public UWcUserWidget
{
	GENERATED_BODY()

public:

	void SetItemDetails(const FWcItemDetail& InItemDetails);

	void SetItemIcon(UTexture2D* Texture) const;

protected:

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon_Image;

private:

	FWcItemDetail ItemDetails;
};
