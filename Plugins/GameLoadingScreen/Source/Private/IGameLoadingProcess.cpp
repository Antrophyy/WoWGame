// Copyright (C) Grip Studios. All Rights Reserved

#include "IGameLoadingProcess.h"

bool IGameLoadingProcess::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	bool bShouldShowLoadingScreen = false;
	FString Reason;

	if (IsValid(TestObject) && TestObject->Implements<UGameLoadingProcess>())
	{
		bShouldShowLoadingScreen = IGameLoadingProcess::Execute_BP_ShouldShowLoadingScreen(TestObject, Reason);

		if (!bShouldShowLoadingScreen)
		{
			if (const IGameLoadingProcess* LoadObserver = Cast<IGameLoadingProcess>(TestObject))
			{
				bShouldShowLoadingScreen = LoadObserver->NativeShouldShowLoadingScreen(Reason);
			}
		}
	}

	if (bShouldShowLoadingScreen)
	{
		ensureMsgf(!Reason.IsEmpty(), TEXT("%s failed to set a reason why it wants to show the loading screen"), *GetPathNameSafe(TestObject));
		Reason = OutReason;
	}

	return bShouldShowLoadingScreen;
}
