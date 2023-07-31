// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Data/AbilityDataAsset.h"
#include "Util/AbilityDragDropOperation.h"
#include "Enums/ItemSlotID.h"
#include "ItemSlot.generated.h"

USTRUCT()
struct FItemSlotData
{
	GENERATED_BODY()

	FName ItemName;
	uint32 ItemAmount;
	FGameplayTag AbilityTrigger;
	FGameplayTag CooldownTag;
	FText Tooltip;
	UPROPERTY()
	class UTexture2D* Icon;
	uint8 bIsGear : 1;

	FItemSlotData() :
		ItemName(NAME_None),
		ItemAmount(0),
		AbilityTrigger(FGameplayTag()),
		CooldownTag(FGameplayTag()),
		Tooltip(FText::FromString("")),
		Icon(nullptr),
		bIsGear(false)
	{ }
};

UCLASS()
class RADIANT_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	EItemSlotID SlotID;
	
	uint8 bIsOnCooldown : 1;
	uint8 bIsEmpty : 1;

	FItemSlotData ItemSlotData;
	
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance{};
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;
	UPROPERTY(meta=(BindWidget))
	class UImage* CooldownMask;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AmountText;

	uint32 ItemAmount = 0;

	FGameplayTag CooldownTag;
	FGameplayTag Trigger;

	uint8 bShouldDropItem : 1;

	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	bool SwapWith(UItemSlot* ItemSlot);
	
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
	virtual void SetData(const FItemSlotData& Data);
	void UpdateCooldown();
	void Reset();

	FItemSlotData& GetItemSlotData() { return ItemSlotData; }

	void SetShouldDropItem(const bool ShouldDropItem) { bShouldDropItem = ShouldDropItem; }
	FName& GetItemName() { return ItemSlotData.ItemName; }

	virtual void SetEmpty(const bool Empty) { bIsEmpty = Empty; }
	bool IsEmpty() const { return bIsEmpty; }

	void SetSlotID(const EItemSlotID& ID) { SlotID = ID; }
	FGameplayTag GetAbilityTrigger();
};
