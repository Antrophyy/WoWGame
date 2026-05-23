// Copyright (C) Grip Studios. All Rights Reserved

#include "Modals/RareUIParameters.h"

#include "LogRareUI.h"

void FRareUIParameters::AddInt32Parameter(const FString& Key, const int32 Value)
{
	Int32Map.Emplace(Key, Value);
}

void FRareUIParameters::AddFloatParameter(const FString& Key, const float Value)
{
	FloatMap.Emplace(Key, Value);
}

void FRareUIParameters::AddStringParameter(const FString& Key, const FString& Value)
{
	StringMap.Emplace(Key, Value);
}

void FRareUIParameters::AddTextParameter(const FString& Key, const FText& Value)
{
	TextMap.Emplace(Key, Value);
}

int32 FRareUIParameters::GetInt32Parameter(const FString& Key) const
{
	if (Int32Map.Contains(Key))
	{
		return Int32Map.FindRef(Key);
	}

	UE_LOG(LogRareUI, Error, TEXT("Could not find a UI parameter with a key %s."), *Key);
	return 0;
}

float FRareUIParameters::GetFloatParameter(const FString& Key) const
{
	if (FloatMap.Contains(Key))
	{
		return FloatMap.FindRef(Key);
	}

	UE_LOG(LogRareUI, Error, TEXT("Could not find a UI parameter with a key %s."), *Key);
	return 0.f;
}

FString FRareUIParameters::GetStringParameter(const FString& Key) const
{
	if (StringMap.Contains(Key))
	{
		return StringMap.FindRef(Key);
	}

	UE_LOG(LogRareUI, Error, TEXT("Could not find a UI parameter with a key %s."), *Key);
	return TEXT("");
}

FText FRareUIParameters::GetTextParameter(const FString& Key) const
{
	if (TextMap.Contains(Key))
	{
		return TextMap.FindRef(Key);
	}

	UE_LOG(LogRareUI, Error, TEXT("Could not find a UI parameter with a key %s."), *Key);
	return FText();
}
