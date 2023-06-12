// Copyright Radiant Studios


#include "DamageCalculation.h"

#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"

UDamageCalculation::UDamageCalculation()
{
	DamageDef.AttributeToCapture = URTHeroAttributeSetBase::GetDamageAttribute();
	DamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	DamageDef.bSnapshot = true;

	RelevantAttributesToCapture.Add(DamageDef);
}

float UDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.f;
	GetCapturedAttributeMagnitude(DamageDef, Spec, EvaluationParameters, Damage);
	Damage = FMath::Max<float>(Damage, 0.0f);

	return -Damage;
}