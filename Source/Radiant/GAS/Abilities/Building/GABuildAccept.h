// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RTAbility.h"
#include "Enums/EnvironmentType.h"
#include "GABuildAccept.generated.h"

UCLASS()
class RADIANT_API UGABuildAccept : public URTAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EEnvironmentType EnvironmentType;
	
public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
