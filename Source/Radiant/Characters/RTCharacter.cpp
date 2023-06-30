// Copyright Radiant Studios


#include "Characters/RTCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARTCharacter::ARTCharacter()
{
	bReplicates = true;
}

void ARTCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTCharacter, TeamId);
}

TArray<TSubclassOf<UGameplayAbility>> ARTCharacter::GetDeathAbilities() const
{
	return DeathAbilities;
}

void ARTCharacter::M_SetIgnoreWalls_Implementation(const bool bIgnoreWalls)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, bIgnoreWalls ? ECR_Ignore : ECR_Block);
}

