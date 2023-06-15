// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GARespawn.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UGARespawn : public UGameplayAbility
{
	GENERATED_BODY()

	UGARespawn();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
