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
	class UTextBlock* MoveSpeed;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Damage;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AttackSpeed;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AttackRange;
	
public:
	void UpdateMovementSpeed(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateDamage(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateAttackSpeed(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateAttackRange(const FOnAttributeChangeData& OnAttributeChangeData);
	void Bind(ARTPlayerState* PlayerState);
};
