#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAAnimated.h"
#include "GAAnimatedApplyToSelf.generated.h"

UCLASS()
class RADIANT_API UGAAnimatedApplyToSelf : public UGAAnimated
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UGameplayEffect>> Effects;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
