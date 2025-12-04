#pragma once

#include "Engine/Texture2D.h"
#include "Foundation/WUserWidget.h"
#include "Items/WcItemDetail.h"
#include "WItemWidget.generated.h"

class UImage;
struct FWcItemDetail;

UCLASS(Abstract, ClassGroup=UI)
class WOWUI_API UWItemWidget : public UWUserWidget
{
	GENERATED_BODY()

public:
	void SetItemDetails(const FWcItemDetail& InItemDetails);
	void SetItemIcon(UTexture2D* Texture) const;

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
private: // Internal
	
	FWcItemDetail ItemDetails;

protected: // Widget Bindings

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon_Image;
};
