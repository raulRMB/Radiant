// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RTAbility.h"
#include "GAMove.generated.h"


UCLASS()
class RADIANT_API UGAMove : public URTAbility
{
	GENERATED_BODY()

public:
	UGAMove();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnTargetLocationReached(bool bGoalReached);
};
