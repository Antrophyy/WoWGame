#include "IGameLoadingProcess.h"

bool IGameLoadingProcess::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	if (TestObject == nullptr)
	{
		return false;
	}

	if (const IGameLoadingProcess* LoadObserver = Cast<IGameLoadingProcess>(TestObject))
	{
		FString ObserverReason;
		if (LoadObserver->ShouldShowLoadingScreen(ObserverReason))
		{
			if (ensureAlwaysMsgf(!ObserverReason.IsEmpty(), TEXT("%s failed to set a reason why it wants to show the loading screen"), *GetPathNameSafe(TestObject)))
			{
				OutReason = ObserverReason;
			}
			
			return true;
		}
	}

	return false;
}
