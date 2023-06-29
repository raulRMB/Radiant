// Copyright Radiant Studios


#include "Characters/RTCharacter.h"
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

