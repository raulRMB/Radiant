// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/ItemSlotID.h"
#include "UI/ItemSlot.h"
#include "UI/RTInfoPanel.h"
#include "UI/InGame/InGameStore.h"
#include "SlotManager.generated.h"

UCLASS(Blueprintable)
class RADIANT_API USlotManager : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<EItemSlotID, class UItemSlot*> Slots;

	UPROPERTY()
	class UUniformGridPanel* InventoryGridPanel;

	UPROPERTY()
	class UHorizontalBox* HotbarHorizontalBox;

	UPROPERTY()
	class UUniformGridPanel* GearGridPanel;

	UPROPERTY()
	class UItemSlot* WeaponSlot;
public:
	UFUNCTION()
	void OnSlotChanged(const FName& Name, uint32 Amount) const;
	void InitSlots(class URTInfoPanel* InfoPanel, UUniformGridPanel* GridPanel, TSubclassOf<UItemSlot> ItemSlotClass, UUniformGridPanel* InGearGridPanel);
	FGameplayTag GetAbilityTrigger(EItemSlotID Slot);

private:
	UItemSlot* GetSlot(const FName& Name) const;
	UItemSlot* GetSlot(EItemSlotID UISlotID) const;
	UItemSlot* FindEmptySlot() const;
	FItemSlotData CreateSlotData(const FName& Name, uint32 Amount) const;
	UItemSlot* CreateNewSlot(const EItemSlotID& UISlotID, TSubclassOf<UItemSlot> ItemSlotClass);
};
