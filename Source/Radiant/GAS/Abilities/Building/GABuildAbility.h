// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RTAbility.h"
#include "GABuildAbility.generated.h"

class ABuilding;
/**
 * 
 */
UCLASS()
class RADIANT_API UGABuildAbility : public URTAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> BuildingClass;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
