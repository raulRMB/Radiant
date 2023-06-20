// Copyright Radiant Studios


#include "Util/Interfaces/Killable.h"

#include "AbilitySystemInterface.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

// Add default functionality here for any IKillable functions that are not pure virtual.
void IKillable::Die()
{
	OnUnitDied.Broadcast();
	
	UAbilitySystemComponent* ASC = Cast<IAbilitySystemInterface>(this)->GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->ClearAllAbilities();
		GiveDeathAbilities();
		if(AAvatar* Avatar = Cast<AAvatar>(this))
			Avatar->S_StopMovement();
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
