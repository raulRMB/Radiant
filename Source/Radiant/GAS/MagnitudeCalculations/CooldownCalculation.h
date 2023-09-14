// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "CooldownCalculation.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UCooldownCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

	FGameplayEffectAttributeCaptureDefinition CooldownReductionDef;

	UCooldownCalculation();
	
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const;
};
