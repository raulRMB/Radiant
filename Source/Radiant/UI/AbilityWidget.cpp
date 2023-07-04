// Copyright Radiant Studios


#include "UI/AbilityWidget.h"

#include "Components/Image.h"
#include "Data/AbilityDataAsset.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Player/RTPlayerState.h"

void UAbilityWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MaterialInstance = UMaterialInstanceDynamic::Create(Mat, this);
	AbilityCDMask->SetBrushResourceObject(MaterialInstance);
	bOn = false;
}

void UAbilityWidget::SetOn(bool On)
{
	bOn = On;
	if(bOn)
		Ability->SetColorAndOpacity(FLinearColor::Gray);
	else
		Ability->SetColorAndOpacity(FLinearColor::White);
	AbilityCDMask->SetVisibility(static_cast<ESlateVisibility>(!bOn));
}

void UAbilityWidget::SetAbilityCoolDown(const float Percent)
{
	MaterialInstance->SetScalarParameterValue("Percent", Percent);
}

float UAbilityWidget::GetCooldownPercent(const float TimeRemaining, const float CooldownDuration)
{
	return (CooldownDuration - TimeRemaining) / CooldownDuration;
}

void UAbilityWidget::SetData(UAbilityDataAsset * Data)
{
    Ability->SetBrushFromTexture(Data->Icon);
    Ability->SetToolTipText(Data->Tooltip);
    AbilityCDMask->SetToolTipText(Data->Tooltip);
	CooldownTag = Data->CooldownTag;
}

void UAbilityWidget::UpdateCooldown()
{
	float TimeRemaining = 0.f;
	float CooldownDuration = 0.f;
	SetOn(false);
	if(GetCooldownRemaining(TimeRemaining, CooldownDuration))
	{
		float Percent = GetCooldownPercent(TimeRemaining, CooldownDuration);
		SetAbilityCoolDown(Percent);
		SetOn(true);
	}
}

bool UAbilityWidget::GetCooldownRemaining(float& TimeRemaining,
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