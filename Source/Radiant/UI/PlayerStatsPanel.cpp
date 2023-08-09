// Copyright Radiant Studios


#include "UI/PlayerStatsPanel.h"

#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "..\Data\TooltipData.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "Structs/TooltipStatInfo.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UPlayerStatsPanel::UpdateMovementSpeed(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MovementSpeedText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateAttackDamage(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttackDamageText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateArmorPenetration(const FOnAttributeChangeData& OnAttributeChangeData)
{
	ArmorPenetrationText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateAbilityPower(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AbilityPowerText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateMagicPenetration(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MagicPenetrationText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateArmor(const FOnAttributeChangeData& OnAttributeChangeData)
{
	ArmorText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateMagicResistance(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MagicResistanceText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateCooldownReduction(const FOnAttributeChangeData& OnAttributeChangeData)
{
	CooldownReductionText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateAttackSpeed(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttackSpeedText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::UpdateAttackRange(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttackRangeText->SetText(RTPRINTF("%.0f", OnAttributeChangeData.NewValue));
}

void UPlayerStatsPanel::Bind(ARTPlayerState* PS)
{
	BindStatsPanelStat(AttackDamage);
	BindStatsPanelStat(ArmorPenetration);
	BindStatsPanelStat(AttackSpeed);
	
	BindStatsPanelStat(AbilityPower);
	BindStatsPanelStat(MagicPenetration);
	BindStatsPanelStat(CooldownReduction);
	
	BindStatsPanelStat(Armor);
	BindStatsPanelStat(MagicResistance);

	BindStatsPanelStat(MovementSpeed);
	BindStatsPanelStat(AttackRange);
}
