// Copyright Radiant Studios


#include "UI/PlayerStatsPanel.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "Player/RTPlayerState.h"

void UPlayerStatsPanel::UpdateMovementSpeed(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MoveSpeed->SetText(FText::FromString(FString::Printf(TEXT("MS: %.0f"), OnAttributeChangeData.NewValue)));
}

void UPlayerStatsPanel::UpdateDamage(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Damage->SetText(FText::FromString(FString::Printf(TEXT("DMG: %.0f"), OnAttributeChangeData.NewValue)));
}

void UPlayerStatsPanel::UpdateAttackSpeed(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttackSpeed->SetText(FText::FromString(FString::Printf(TEXT("AS: %.0f"), OnAttributeChangeData.NewValue)));
}

void UPlayerStatsPanel::UpdateAttackRange(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttackRange->SetText(FText::FromString(FString::Printf(TEXT("Range: %.0f"), OnAttributeChangeData.NewValue)));
}

void UPlayerStatsPanel::Bind(ARTPlayerState* PS)
{
	PS->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PS->GetAttributeSetBase()->GetMovementSpeedAttribute()).AddUObject(
		this, &UPlayerStatsPanel::UpdateMovementSpeed);
	MoveSpeed->SetText(FText::FromString(FString::Printf(TEXT("MS: %.0f"), PS->GetAttributeSetBase()->GetMovementSpeed())));
	PS->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PS->GetAttributeSetBase()->GetAttackDamageAttribute()).AddUObject(
		this, &UPlayerStatsPanel::UpdateDamage);
	Damage->SetText(FText::FromString(FString::Printf(TEXT("DMG: %.0f"), PS->GetAttributeSetBase()->GetAttackDamage())));
	PS->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PS->GetAttributeSetBase()->GetAttackSpeedAttribute()).AddUObject(
		this, &UPlayerStatsPanel::UpdateAttackSpeed);
	AttackSpeed->SetText(FText::FromString(FString::Printf(TEXT("AS: %.0f"), PS->GetAttributeSetBase()->GetAttackSpeed())));
	PS->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PS->GetAttributeSetBase()->GetAttackRangeAttribute()).AddUObject(
		this, &UPlayerStatsPanel::UpdateAttackRange);
	AttackRange->SetText(FText::FromString(FString::Printf(TEXT("AR: %.0f"), PS->GetAttributeSetBase()->GetAttackRange())));
}
