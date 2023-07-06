// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GALinearSkillshot.h"
#include "GAJook.generated.h"


UCLASS()
class RADIANT_API UGAJook : public UGALinearSkillshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class AActor* Target;
		
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void OnTargetHit(class AActor* NewTarget);
private:

	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	
	UFUNCTION()
	void OnFinished();

	UPROPERTY(EditAnywhere)
	double DisplaceSpeed = 2000.0f;
};
