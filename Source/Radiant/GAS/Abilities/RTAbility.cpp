// Copyright Radiant Studios


#include "GAS/Abilities/RTAbility.h"

FVector URTAbility::GetRangeBasedMouseLocation()
{
	FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector MouseLocation = GetMouseWorldLocation();
	FVector Direction = MouseLocation - AvatarLocation;
	
	return Direction.GetClampedToSize(0.0, Range) + AvatarLocation;
}
