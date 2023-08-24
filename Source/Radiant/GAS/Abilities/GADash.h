// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAAnimated.h"
#include "GADash.generated.h"

UENUM(BlueprintType)
enum class EDashType : uint8
{
	TeleportTimed,
	TeleportInstant,
	Linear,
	Parabolic
};


UCLASS()
class RADIANT_API UGADash : public UGAAnimated
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	EDashType DashType;
	
	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true", EditCondition="DashType == EDashType::Linear || DashType == EDashType::Parabolic", EditConditionHides))
	float DashTime;

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UCurveVector> ParabolicCurve;

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	float CurveMultiplier = 1000.f;
	
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
protected:
	UFUNCTION()
	void OnTargetLocationReached();
private:
	void LinearDash();
	void ParabolicDash();
	void TeleportInstant();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
