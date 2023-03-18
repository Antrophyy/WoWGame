#include "Core/WcGameGameMode.h"
#include "Characters/WcGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWcGameGameMode::AWcGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
