#include "HUD/IndicatorSystem/Components/WcIndicatorComponent_Base.h"
#include "GameFramework/PlayerController.h"

void UWcIndicatorComponent_Base::InitializeComponent(APlayerController* InOwningPlayer, const TSoftClassPtr<UBaseIndicatorWidget> InIndicatorWidgetClass)
{
	OwningPlayer = InOwningPlayer;
	IndicatorWidgetClass = InIndicatorWidgetClass;

	ensure(OwningPlayer.IsValid());
	ensure(!IndicatorWidgetClass.IsNull());

	NativeComponentInitialized();
}
