#include "Game/HUD/IndicatorSystem/Handlers/WIndicatorHandler_Base.h"
#include "GameFramework/PlayerController.h"

void UWIndicatorHandler_Base::InitializeComponent(APlayerController* InOwningPlayer, const TSoftClassPtr<UGameBaseIndicatorWidget> InIndicatorWidgetClass)
{
	OwningPlayer = InOwningPlayer;
	IndicatorWidgetClass = InIndicatorWidgetClass;

	ensure(OwningPlayer.IsValid());
	ensure(!IndicatorWidgetClass.IsNull());

	NativeComponentInitialized();
}
