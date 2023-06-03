// Copyright Radiant Studios


#include "GAS/Abilities/RTAbility.h"

#include "Character/Hero.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"

void URTAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FVector URTAbility::GetRangeBasedMouseLocation()
{
	FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector MouseLocation = GetMouseWorldLocation();
	FVector Direction = (MouseLocation - AvatarLocation) * FVector(1, 1, 0);
	
	return Direction.GetClampedToSize(0.0, MaxRange) + AvatarLocation * FVector(1, 1, 0);
}

FVector URTAbility::GetRangedBaseMouseLocationWithHeroHalfHeight()
{
	if(AHero* Hero = Cast<AHero>(GetAvatarActorFromActorInfo()))
	{
		return GetRangeBasedMouseLocation() + Hero->GetHalfHeightVector();
	}
	LOG("Avatar is not a hero")
	PRINT("Avatar is not a hero")
	return FVector::ZeroVector;
}
