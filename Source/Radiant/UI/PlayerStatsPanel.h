// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatsPanel.generated.h"

class ARTPlayerState;
/**
 * 
 */
UCLASS()
class RADIANT_API UPlayerStatsPanel : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UImage* AttackDamageIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AttackDamageText;

	UPROPERTY(meta=(BindWidget))
	class UImage* ArmorPenetrationIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ArmorPenetrationText;

	UPROPERTY(meta=(BindWidget))
	class UImage* AttackSpeedIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AttackSpeedText;
	

	UPROPERTY(meta=(BindWidget))
	class UImage* AbilityPowerIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AbilityPowerText;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* MagicPenetrationIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MagicPenetrationText;

	UPROPERTY(meta=(BindWidget))
	class UImage* CooldownReductionIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownReductionText;
	

	UPROPERTY(meta=(BindWidget))
	class UImage* ArmorIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ArmorText;

	UPROPERTY(meta=(BindWidget))
	class UImage* MagicResistanceIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MagicResistanceText;

	
	UPROPERTY(meta=(BindWidget))
	class UImage* AttackRangeIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AttackRangeText;

	UPROPERTY(meta=(BindWidget))
	class UImage* MovementSpeedIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MovementSpeedText;

public:
	void UpdateAttackDamage(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateArmorPenetration(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateAttackSpeed(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateAbilityPower(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateMagicPenetration(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateArmor(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateMagicResistance(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateCooldownReduction(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateMovementSpeed(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateAttackRange(const FOnAttributeChangeData& OnAttributeChangeData);
	void Bind(ARTPlayerState* PlayerState);
};
