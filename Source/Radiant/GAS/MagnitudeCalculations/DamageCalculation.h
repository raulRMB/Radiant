// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UDamageCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

private:
	UDamageCalculation();

	FGameplayEffectAttributeCaptureDefinition HealthDef;
	FGameplayEffectAttributeCaptureDefinition DamageDef;
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
