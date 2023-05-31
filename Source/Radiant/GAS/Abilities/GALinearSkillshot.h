// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAAnimated.h"
#include "GALinearSkillshot.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UGALinearSkillshot : public UGAAnimated
{
	GENERATED_BODY()
public:
	UGALinearSkillshot();
private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
};
