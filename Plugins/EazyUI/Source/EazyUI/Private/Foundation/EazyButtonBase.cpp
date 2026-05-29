#include "Foundation/EazyButtonBase.h"

void UEazyButtonBase::NativeConstruct()
{
	Super::NativeConstruct();

	NativeResetAnimationsState();
}

void UEazyButtonBase::ForceClick()
{
	HandleButtonClicked();
}

void UEazyButtonBase::ForceHover()
{
	NativeOnHovered();
}

void UEazyButtonBase::ForceUnhover()
{
	NativeOnUnhovered();
}

void UEazyButtonBase::NativeResetAnimationsState()
{
	StopAllAnimations();
	FlushAnimations();

	BP_ResetAnimationsState();
}
