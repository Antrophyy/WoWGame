#include "Frontend/WcMainMenuLevel.h"
#include "PrimaryGameLayout.h"
#include "Kismet/GameplayStatics.h"
#include "UINativeTags.h"

void AWcMainMenuLevel::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
		{
			RootLayout->PushWidgetToLayerAsync(TAG_UI_LAYER_MENUSCREEN, true, MainMenuScreenClass);
		}
	}
}
