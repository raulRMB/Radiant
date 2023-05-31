// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GAAnimated.h"
#include "GAAreaOfEffect.generated.h"

UENUM(BlueprintType)
enum class ESpawnBehavior : uint8
{
	AtMouseLocation = 0,
	AtAvatarLocation,
	AtRangeLocation
};

/**
 * 
 */
UCLASS()
class RADIANT_API UGAAreaOfEffect : public UGAAnimated
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	uint8 bInstantCast : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	ESpawnBehavior SpawnBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FRotator SpawnRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	uint8 bMatchAvatarRotation : 1;
public:
	UGAAreaOfEffect();
protected:
	virtual void SpawnAreaOfEffect();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
};
