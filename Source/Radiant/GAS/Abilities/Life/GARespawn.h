// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RTAbility.h"
#include "GARespawn.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UGARespawn : public URTAbility
{
	GENERATED_BODY()

	UGARespawn();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
