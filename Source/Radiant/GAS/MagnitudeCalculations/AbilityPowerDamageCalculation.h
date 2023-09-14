// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "AbilityPowerDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UAbilityPowerDamageCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

private:
	UAbilityPowerDamageCalculation();

	FGameplayEffectAttributeCaptureDefinition MagicResistanceDef;
	FGameplayEffectAttributeCaptureDefinition AbilityPowerDef;
	FGameplayEffectAttributeCaptureDefinition MagicPenetrationDef;

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
