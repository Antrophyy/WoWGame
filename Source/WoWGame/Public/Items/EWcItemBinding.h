#pragma once

UENUM(BlueprintType)
enum class EWcItemBinding : uint8
{
	BindOnPickup,
	BindOnAcquire,
	BindOnUse,
	BindToAccount
};
