// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GADash.h"
#include "DPS_Blink.generated.h"


UCLASS()
class RADIANT_API UDPS_Blink : public UGADash
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	FGameplayTag BlinkStartCue;
	
	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	FGameplayTag BlinkEndCue;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
