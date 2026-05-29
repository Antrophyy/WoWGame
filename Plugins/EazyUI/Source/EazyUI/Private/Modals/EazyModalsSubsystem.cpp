#include "Modals/EazyModalsSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"

bool UEazyModalsSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<ULocalPlayer>(Outer)->GetGameInstance()->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}
