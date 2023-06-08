// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Combat/Artillery/HeatSeeking.h"
#include "GAS/Abilities/GAAnimated.h"
#include "BasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UBasicAttack : public UGAAnimated
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<AHeatSeeking> ProjectileClass;
	
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
