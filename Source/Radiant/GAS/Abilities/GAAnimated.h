// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "RTAbility.h"
#include "GAAnimated.generated.h"


UENUM(BlueprintType)
enum class EAnimatedAbilityTarget : uint8
{
	Self,
	Target,
	SelfAndTarget
};

UENUM(BlueprintType)
enum class EAnimatedAbilityCastType : uint8
{
	Instant,
	OnNotify
};

UCLASS()
class RADIANT_API UGAAnimated : public URTAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	uint8 bFireOnInterrupt : 1;
	uint8 bHasFired : 1;

	UPROPERTY(EditAnywhere)
	EAnimatedAbilityTarget AnimatedAbilityTarget;

	UPROPERTY(EditAnywhere)
	EAnimatedAbilityCastType AnimatedAbilityCastType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> InstantEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Radiant.EArtilleryBehavior"))
	int32 TargetingBehavior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	float MontagePlayRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SpawnTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	float SpawnRange = 300.f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	FGameplayTagContainer CueTags;
	
public:
	UGAAnimated();
protected:
	virtual void OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	virtual void OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	virtual void OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData);
	virtual void OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData);
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) {}
	void ReturnToDefaultAndEndAbility(bool bWasCancelled = false);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	bool ShouldHit(AActor* OtherActor);
	void ReturnToDefault() const;
	void BindAnimations(float PlayRate);
	void BindAnimations();
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
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
