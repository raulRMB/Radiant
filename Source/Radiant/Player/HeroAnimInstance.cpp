// Copyright Radiant Studios


#include "HeroAnimInstance.h"

void UHeroAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (TryGetPawnOwner())
    {
        FVector Speed = TryGetPawnOwner()->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
        MovementSpeed = LateralSpeed.Size();
    }
}
