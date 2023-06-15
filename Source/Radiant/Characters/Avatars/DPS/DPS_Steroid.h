// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAInstant.h"
#include "DPS_Steroid.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UDPS_Steroid : public UGAInstant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	FGameplayTag EffectCueTag;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
