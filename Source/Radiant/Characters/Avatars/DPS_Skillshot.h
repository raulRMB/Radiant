// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GALinearSkillshot.h"
#include "DPS_Skillshot.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UDPS_Skillshot : public UGALinearSkillshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	FGameplayTag CastStartCue;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};
