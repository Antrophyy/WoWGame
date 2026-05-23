// Copyright (C) Grip Studios. All Rights Reserved

#include "Modals/RareModalsSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"

bool URareModalsSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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
