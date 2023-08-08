// Copyright Radiant Studios


#include "AttackDamageCalculation.h"
#include "GAS/AttributeSets/RTBaseAttributeSet.h"
#include "Util/Util.h"

UAttackDamageCalculation::UAttackDamageCalculation()
{
	AttackDamageDef.AttributeToCapture = URTBaseAttributeSet::GetAttackDamageAttribute();
	AttackDamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AttackDamageDef.bSnapshot = true;

	ArmorPenetrationDef.AttributeToCapture = URTBaseAttributeSet::GetArmorPenetrationAttribute();
	ArmorPenetrationDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	ArmorPenetrationDef.bSnapshot = true;

	ArmorDef.AttributeToCapture = URTBaseAttributeSet::GetArmorAttribute();
	ArmorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ArmorDef.bSnapshot = true;

	RelevantAttributesToCapture.Add(AttackDamageDef);
	RelevantAttributesToCapture.Add(ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(ArmorDef);
}

float UAttackDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	if(TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("States.Invulnerable"))))
	{
		return 0.f;
	}

	float Damage;
	GetCapturedAttributeMagnitude(AttackDamageDef, Spec, EvaluationParameters, Damage);
	Damage = FMath::Max(Damage, 0.0f);

	float ArmorPenetration = 0.f;
	GetCapturedAttributeMagnitude(ArmorPenetrationDef, Spec, EvaluationParameters, ArmorPenetration);
	ArmorPenetration = FMath::Max(ArmorPenetration, 0.0f);
		
	float ArmorReduction = ArmorPenetration > 0.f ? ArmorPenetration / 100.f : 0.f;

	float NominalArmor = 0.f;
	GetCapturedAttributeMagnitude(ArmorDef, Spec, EvaluationParameters, NominalArmor);
	NominalArmor = FMath::Max(NominalArmor, 0.0f);

	float EffectiveArmor = NominalArmor * (1 - ArmorReduction);

	float MitigatedDamage;
	if(NominalArmor >= 0.f)
	{
		MitigatedDamage = Damage * (100 / (EffectiveArmor + 100.f));
	}
	else
	{
		MitigatedDamage = Damage * (2 - 100 / (100 - EffectiveArmor));
	}

	RTLOGP("Damage: %.2f", MitigatedDamage)
	
	return -MitigatedDamage;
}
