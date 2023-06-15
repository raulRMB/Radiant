// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAAreaOfEffect.h"
#include "DPS_Ultimate.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UDPS_Ultimate : public UGAAreaOfEffect
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
