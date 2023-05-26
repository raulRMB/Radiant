// Copyright Radiant Studios


#include "UI/RTInfoPanel.h"

#include "Character/Hero.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Player/RTPlayerState.h"


class ARTPlayerState;

void URTInfoPanel::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdateProperties(DeltaTime);
}

void URTInfoPanel::SetAbilityCoolDown(EAbilityID Abilty, const float Percent)
{
	Abilities[static_cast<int>(Abilty)].MaterialInstance->SetScalarParameterValue("Percent", Percent);
}

void URTInfoPanel::AbilityCooldowns()
{
	for(int i = 0; i < Tags.Num(); i++)
	{
		float TimeRemaining = 0.f;
		float CooldownDuration = 0.f;
		Abilities[i].SetOn(false);
		if(GetCooldownRemainingForTag(Tags[i], TimeRemaining, CooldownDuration))
		{
			float Percent = GetCooldownPercent(TimeRemaining, CooldownDuration);
			SetAbilityCoolDown(static_cast<EAbilityID>(i), Percent);
			Abilities[i].SetOn(true);
		}
	}
}

void URTInfoPanel::UpdateProperties(float DeltaTime)
{
	if(auto State = GetOwningPlayerState<ARTPlayerState>())
	{
		if(URTHeroAttributeSetBase* Attributes = State->GetAttributeSetBase())
		{
			AbilityCooldowns();

			HealthBar->SetPercent(Attributes->GetHealth() / Attributes->GetMaxHealth());
			ManaBar->SetPercent(Attributes->GetMana() / Attributes->GetMaxMana());
		}
	}
}

void URTInfoPanel::Init()
{
	Abilities.Add(FUIAbility(Mat, this, Ability1, Ability1CDMask));
	Abilities.Add(FUIAbility(Mat, this, Ability2, Ability2CDMask));
	Abilities.Add(FUIAbility(Mat, this, Ability3, Ability3CDMask));
	Abilities.Add(FUIAbility(Mat, this, Ability4, Ability4CDMask));
	Abilities.Add(FUIAbility(Mat, this, Ability5, Ability5CDMask));
	Abilities.Add(FUIAbility(Mat, this, Ability6, Ability6CDMask));

	Ability1CDMask->SetBrushResourceObject(Abilities[static_cast<int>(EAbilityID::Ability1)].MaterialInstance);
	Ability2CDMask->SetBrushResourceObject(Abilities[static_cast<int>(EAbilityID::Ability2)].MaterialInstance);
	Ability3CDMask->SetBrushResourceObject(Abilities[static_cast<int>(EAbilityID::Ability3)].MaterialInstance);
	Ability4CDMask->SetBrushResourceObject(Abilities[static_cast<int>(EAbilityID::Ability4)].MaterialInstance);
	Ability5CDMask->SetBrushResourceObject(Abilities[static_cast<int>(EAbilityID::Ability5)].MaterialInstance);
	Ability6CDMask->SetBrushResourceObject(Abilities[static_cast<int>(EAbilityID::Ability6)].MaterialInstance);
}

bool URTInfoPanel::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining,
	float& CooldownDuration)
{
	URTAbilitySystemComponent* AbilitySystemComponent = Cast<URTAbilitySystemComponent>(GetOwningPlayerState<ARTPlayerState>()->GetAbilitySystemComponent());
	
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}

float URTInfoPanel::GetCooldownPercent(const float TimeRemaining, const float CooldownDuration)
{
	return (CooldownDuration - TimeRemaining) / CooldownDuration;
}