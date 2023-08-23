#pragma once

#include "Blueprint/DragDropOperation.h"
#include "WcDragWindowOperation.generated.h"

class UWcUserWidget;

UCLASS()
class WOWUI_API UWcDragWindowOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWcUserWidget> WidgetReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2f DragOffset;
};
