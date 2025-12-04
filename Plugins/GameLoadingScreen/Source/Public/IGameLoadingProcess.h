#pragma once

#include "UObject/Interface.h"
#include "IGameLoadingProcess.generated.h"

// Interface for things that might cause loading to happen which requires a loading screen to be displayed.
UINTERFACE(Category="UI|LoadingScreen")
class GAMELOADINGSCREEN_API UGameLoadingProcess : public UInterface
{
	GENERATED_BODY()
};

class GAMELOADINGSCREEN_API IGameLoadingProcess
{
	GENERATED_BODY()

public:
	// Checks to see if this object implements the interface, and if so asks whether or not we should
	// be currently showing a loading screen
	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const { return false; }
};
