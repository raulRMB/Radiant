// Copyright Radiant Studios


#include "CooldownCalculation.h"

#include "GAS/AttributeSets/RTBaseAttributeSet.h"

UCooldownCalculation::UCooldownCalculation()
{
	CooldownReductionDef.AttributeToCapture = URTBaseAttributeSet::GetCooldownReductionAttribute();
	CooldownReductionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	CooldownReductionDef.bSnapshot = true;
	
	RelevantAttributesToCapture.Add(CooldownReductionDef);
}

float UCooldownCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float CooldownReduction;
	GetCapturedAttributeMagnitude(CooldownReductionDef, Spec, EvaluationParameters, CooldownReduction);

	float CDR = (100.f - FMath::Clamp(CooldownReduction, 0.0001f, 100.f)) * 0.01f;
	return CDR;
}
