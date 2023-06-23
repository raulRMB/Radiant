// Copyright Radiant Studios


#include "UI/RTInfoPanel.h"

#include "Player/Avatar.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/AbilityDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"


class ARTPlayerState;

void URTInfoPanel::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdateProperties(DeltaTime);
}

void URTInfoPanel::SetCooldownTags(TArray<FGameplayTag> NewCooldownTags)
{
	CooldownTags = NewCooldownTags;
}

void URTInfoPanel::SetAbilityCoolDown(EAbilityID Abilty, const float Percent)
{
	Abilities[static_cast<int>(Abilty)].MaterialInstance->SetScalarParameterValue("Percent", Percent);
}

void URTInfoPanel::AbilityCooldowns()
{
	for(int i = 0; i < CooldownTags.Num(); i++)
	{
		float TimeRemaining = 0.f;
		float CooldownDuration = 0.f;
		Abilities[i].SetOn(false);
		if(GetCooldownRemainingForTag(CooldownTags[i], TimeRemaining, CooldownDuration))
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
			HealthAmount->SetText(FormatText(Attributes->GetHealth(), Attributes->GetMaxHealth()));
			ManaAmount->SetText(FormatText(Attributes->GetMana(), Attributes->GetMaxMana()));
		}
	}
}

FText URTInfoPanel::FormatText(float CurrentHealth, float MaxHealth) const
{
	return FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth));
}

void URTInfoPanel::UpdateAbilities(TArray<UAbilityDataAsset*> AbilityData)
{
	int32 iconCounter = 0;
	TArray<FGameplayTag> NewCooldownTags;
	for(int i = 0; i < AbilityData.Num(); i++)
	{
		if(AbilityData[i]->IgnoreHud)
		{
			continue;
		}
		if(iconCounter < Abilities.Num())
		{
			Abilities[iconCounter].AbilityImage->SetBrushFromTexture(AbilityData[i]->Icon);
			Abilities[iconCounter].AbilityImage->SetToolTipText(AbilityData[i]->Tooltip);
			Abilities[iconCounter].MaskImage->SetToolTipText(AbilityData[i]->Tooltip);
			iconCounter++;
		}
		NewCooldownTags.AddUnique(AbilityData[i]->CooldownTag);
	}
	SetCooldownTags(NewCooldownTags);
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
	Cast<ARTGameState>(UGameplayStatics::GetGameState(this))->OnHeroDeathDelegate.BindUObject(this, &URTInfoPanel::OnHeroDeath);
}

void URTInfoPanel::OnHeroDeath(uint32 RedScore, uint32 BlueScore)
{
	RedTeam->SetText(FText::FromString(FString::FromInt(RedScore)));
	BlueTeam->SetText(FText::FromString(FString::FromInt(BlueScore)));
}
bool URTInfoPanel::GetCooldownRemainingForTag(FGameplayTag CooldownTag, float& TimeRemaining,
	float& CooldownDuration)
{
	URTAbilitySystemComponent* AbilitySystemComponent = Cast<URTAbilitySystemComponent>(GetOwningPlayerState<ARTPlayerState>()->GetAbilitySystemComponent());
	
	if (AbilitySystemComponent && CooldownTag.IsValid())
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(CooldownTag);
		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
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

void URTInfoPanel::ShowEndScreen(bool bWon)
{
	if(bWon)
	{
		Victory->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		Defeat->SetVisibility(ESlateVisibility::Visible);
	}
}

void URTInfoPanel::HideLoadScreen()
{
	if(LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URTInfoPanel::SetFPS(float FPS)
{
	FPSCounter->SetText(FText::FromString(FString::Printf(TEXT("FPS: %.0f"), FPS)));
}

void URTInfoPanel::SetMS(float MS)
{
	MSCounter->SetText(RTPRINTF("MS: %.0f", MS));
}
