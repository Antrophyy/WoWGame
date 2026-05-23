// Copyright (C) Grip Studios. All Rights Reserved

#include "Foundation/RareButtonBase.h"

void URareButtonBase::NativeConstruct()
{
	Super::NativeConstruct();

	NativeResetAnimationsState();
}

void URareButtonBase::ForceClick()
{
	HandleButtonClicked();
}

void URareButtonBase::ForceHover()
{
	NativeOnHovered();
}

void URareButtonBase::ForceUnhover()
{
	NativeOnUnhovered();
}

void URareButtonBase::NativeResetAnimationsState()
{
	StopAllAnimations();
	FlushAnimations();

	BP_ResetAnimationsState();
}
