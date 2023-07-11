// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RTAbility.h"
#include "Util/Managers/GridManager.h"
#include "GABuildAbility.generated.h"

class ABuilding;

UCLASS()
class RADIANT_API UGABuildAbility : public URTAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEnvironmentType EnvironmentType;

	FDelegateHandle OrderAcceptedHandle;

	FGameplayAbilitySpecHandle ChildSpecHandle;

	UPROPERTY(EditAnywhere)
	FName ItemName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URTAbility> BuildAcceptAbility;
	
public:
	UGABuildAbility();

	UFUNCTION()
	void OnOrderAccepted();
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
