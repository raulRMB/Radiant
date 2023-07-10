// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RTAbility.generated.h"


UCLASS()
class RADIANT_API URTAbility : public UGameplayAbility
{
	GENERATED_BODY()

	FVector MouseWorldLocation;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, Category="Ability")
	float MaxRange = 1000.f;
public:
	const FVector& GetMouseWorldLocation() const { return MouseWorldLocation; }
	FVector GetRangeBasedMouseLocation();
	void SetMouseWorldLocation(const FVector& Location) { MouseWorldLocation = Location; }
	FVector GetRangedBaseMouseLocationWithHeroHalfHeight();
	FVector GetAvatarToMouseDirection();
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	static void GetMatchingTags(const FGameplayTagContainer& Tags1, const FGameplayTagContainer& Tags2,
	                            FGameplayTagContainer* OptionalRelevantTags);
	static void GetMissingTags(const FGameplayTagContainer& Tags1, const FGameplayTagContainer& Tags2,
	                           FGameplayTagContainer* OptionalRelevantTags);
	FGameplayTag GetTriggerTag() const;
	FGameplayTag GetCooldownTag() const;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	void UseItem(const FGameplayAbilitySpecHandle& Handle);
};
