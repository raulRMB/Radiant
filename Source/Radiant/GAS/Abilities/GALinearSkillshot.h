// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GALinearSkillshot.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UGALinearSkillshot : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	class AActor* Instigator;
	
public:
	UGALinearSkillshot();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnTargetValidData(const FGameplayAbilityTargetDataHandle& Data);
	
};
