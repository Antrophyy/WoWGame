// Copyright (C) Grip Studios. All Rights Reserved

#pragma once

#include "RareUIParameters.generated.h"

// This struct is used for any parameters that UI may want to pass around. The usage is quite simple,
// wherever you are allowed to pass RareUIParameters, you can add any parameters and retrieve them
// without having to specify these parameters explicitly.
USTRUCT(Category=UI)
struct RAREUI_API FRareUIParameters
{
	GENERATED_BODY()

public:
	void AddInt32Parameter(const FString& Key, const int32 Value);
	int32 GetInt32Parameter(const FString& Key) const;
	
	void AddFloatParameter(const FString& Key, const float Value);
	float GetFloatParameter(const FString& Key) const;
	
	void AddStringParameter(const FString& Key, const FString& Value);
	FString GetStringParameter(const FString& Key) const;
	
	void AddTextParameter(const FString& Key, const FText& Value);
	FText GetTextParameter(const FString& Key) const;

private: // Internal

	TMap<FString, int32> Int32Map;
	TMap<FString, float> FloatMap;
	TMap<FString, FString> StringMap;
	TMap<FString, FText> TextMap;
};
