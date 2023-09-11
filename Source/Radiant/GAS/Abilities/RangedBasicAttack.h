// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Combat/Artillery/HeatSeeking.h"
#include "GAS/Abilities/GAAnimated.h"
#include "RangedBasicAttack.generated.h"


UCLASS()
class RADIANT_API URangedBasicAttack : public UGAAnimated
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<AHeatSeeking> ProjectileClass;

	UPROPERTY(EditAnywhere)
	FName SocketName = FName("LeftHandSocket");

	UPROPERTY(EditAnywhere)
	class AActor* Target;

	virtual void OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData) override;
	
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
