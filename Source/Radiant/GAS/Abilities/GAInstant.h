// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAInstant.generated.h"

UENUM(BlueprintType)
enum class EInstantAbilityTarget : uint8
{
	Self,
	Target,
	SelfAndTarget
};

/**
 * 
 */
UCLASS()
class RADIANT_API UGAInstant : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<class UGameplayEffect>> GameplayEffects;

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	EInstantAbilityTarget AbilityTarget;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};