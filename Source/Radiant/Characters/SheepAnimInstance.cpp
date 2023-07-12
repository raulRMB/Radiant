// Copyright Radiant Studios

#include "Characters/SheepAnimInstance.h"

void USheepAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (TryGetPawnOwner())
	{
		MovementSpeed = TryGetPawnOwner()->GetVelocity().Size();
	}
}
