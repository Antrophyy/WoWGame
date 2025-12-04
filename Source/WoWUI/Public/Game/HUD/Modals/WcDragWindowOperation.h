#pragma once

#include "Blueprint/DragDropOperation.h"
#include "WcDragWindowOperation.generated.h"

class UWUserWidget;

UCLASS()
class WOWUI_API UWcDragWindowOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWUserWidget> WidgetReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2f DragOffset;
};
