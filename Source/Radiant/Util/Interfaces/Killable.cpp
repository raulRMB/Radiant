// Copyright Radiant Studios


#include "Util/Interfaces/Killable.h"

#include "AbilitySystemInterface.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

void IKillable::SetIsDead(const bool bIsDead)
{
	if(bIsDead)
	{
		OnUnitDied.Broadcast();
	}
}
