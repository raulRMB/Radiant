// Copyright Radiant Studios


#include "Interfaces/Killable.h"

#include "AbilitySystemInterface.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Util/Util.h"

// Add default functionality here for any IKillable functions that are not pure virtual.
void IKillable::Die()
{
	UAbilitySystemComponent* ASC = Cast<IAbilitySystemInterface>(this)->GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->ClearAllAbilities();
		GiveDeathAbilities();
	}
}

void IKillable::GiveDeathAbilities()
{
	UAbilitySystemComponent* ASC = Cast<IAbilitySystemInterface>(this)->GetAbilitySystemComponent();
	if(ASC)
	{
		for(auto Ability : GetDeathAbilities())
		{
			ASC->GiveAbility(Ability);
		}
	}
}
