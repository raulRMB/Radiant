// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAAnimated.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UGAAnimated : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	float MontagePlayRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SelfTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SpawnTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	float SpawnRange = 300.f;
private:
	FVector MouseWorldLocation;
public:
	UGAAnimated();
	const FVector& GetMouseWorldLocation() const { return MouseWorldLocation; }
protected:
	virtual void OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData) {}
	virtual void OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData) {}
	virtual void OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData) {}
	virtual void OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData) {}
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) {}
	void ReturnToDefaultAndEndAbility(bool bWasCancelled = false);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
	void OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
	void OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
