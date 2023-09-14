// Copyright Radiant Studios


#include "AbilityPowerDamageCalculation.h"

#include "GAS/AttributeSets/RTBaseAttributeSet.h"

UAbilityPowerDamageCalculation::UAbilityPowerDamageCalculation()
{
	AbilityPowerDef.AttributeToCapture = URTBaseAttributeSet::GetAbilityPowerAttribute();
	AbilityPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AbilityPowerDef.bSnapshot = false;

	MagicPenetrationDef.AttributeToCapture = URTBaseAttributeSet::GetMagicPenetrationAttribute();
	MagicPenetrationDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	MagicPenetrationDef.bSnapshot = false;

	MagicResistanceDef.AttributeToCapture = URTBaseAttributeSet::GetMagicResistanceAttribute();
	MagicResistanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MagicResistanceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AbilityPowerDef);
	RelevantAttributesToCapture.Add(MagicPenetrationDef);
	RelevantAttributesToCapture.Add(MagicResistanceDef);
}


float UAbilityPowerDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	if(TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("States.Invulnerable")))
		|| TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("States.Dead"))))
	{
		return 0.f;
	}

	float Damage;
	GetCapturedAttributeMagnitude(AbilityPowerDef, Spec, EvaluationParameters, Damage);
	Damage = FMath::Max(Damage, 0.0f);

	float Penetration = 0.f;
	GetCapturedAttributeMagnitude(MagicPenetrationDef, Spec, EvaluationParameters, Penetration);
	Penetration = FMath::Max(Penetration, 0.0f);
		
	float Reduction = Penetration > 0.f ? Penetration / 100.f : 0.f;

	float NominalResistance = 0.f;
	GetCapturedAttributeMagnitude(MagicResistanceDef, Spec, EvaluationParameters, NominalResistance);
	NominalResistance = FMath::Max(NominalResistance, 0.0f);

	float EffectiveResistance = NominalResistance * (1 - Reduction);

	float MitigatedDamage;
	if(NominalResistance >= 0.f)
	{
		MitigatedDamage = Damage * (100 / (EffectiveResistance + 100.f));
	}
	else
	{
		MitigatedDamage = Damage * (2 - 100 / (100 - EffectiveResistance));
	}
	
	return -MitigatedDamage;
}
