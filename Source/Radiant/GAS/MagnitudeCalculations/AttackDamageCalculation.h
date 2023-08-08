// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "AttackDamageCalculation.generated.h"


UCLASS()
class RADIANT_API UAttackDamageCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

private:
	UAttackDamageCalculation();

	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	FGameplayEffectAttributeCaptureDefinition AttackDamageDef;
	FGameplayEffectAttributeCaptureDefinition ArmorPenetrationDef;
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
