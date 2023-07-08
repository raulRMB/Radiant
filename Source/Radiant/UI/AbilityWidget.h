// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Data/AbilityDataAsset.h"
#include "Util/Enums/InventorySlot.h"
#include "AbilityWidget.generated.h"

UCLASS()
class RADIANT_API UAbilityWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EInventorySlot HotbarSlot;
	
	uint8 bOn : 1;
	UPROPERTY()
	UAbilityDataAsset* AbilityData;
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance{};
	UPROPERTY(meta=(BindWidget))
	class UImage* Ability;
	UPROPERTY(meta=(BindWidget))
	class UImage* AbilityCDMask;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AmountText;

	uint32 ItemAmount = 0;

	UPROPERTY(EditAnywhere, Category="Cooldowns")
	FGameplayTag CooldownTag;

	uint8 bShouldDropItem : 1;

	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                  UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void SetOn(bool On);
	float GetCooldownPercent(const float TimeRemaining, const float CooldownDuration);
	bool GetCooldownRemaining(float& TimeRemaining, float& CooldownDuration);
	void SetAbilityCoolDown(const float Percent);
public:
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* Mat;
	void SetData(UAbilityDataAsset* Data, uint32 Amount);
	void UpdateCooldown();
	void Reset();

	void SetShouldDropItem(const bool ShouldDropItem) { bShouldDropItem = ShouldDropItem; }
};
