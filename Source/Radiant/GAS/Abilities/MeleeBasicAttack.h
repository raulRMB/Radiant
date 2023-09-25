// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAAnimated.h"
#include "MeleeBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UMeleeBasicAttack : public UGAAnimated
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> MovementStoppedEffect;

	FActiveGameplayEffectHandle MovementStoppedEffectHandle;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UGameplayEffect>> Effects;
	uint8 bHasRemovedMovementStoppedEffect : 1;

	virtual void OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData) override;
	
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	UFUNCTION()
	void OnSetUncancelable(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnUnsetUncancelable(FGameplayEventData Payload);

	UFUNCTION()
	void RemoveMovementStoppedEffect();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
