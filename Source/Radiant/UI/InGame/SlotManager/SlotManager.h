// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/ItemSlotID.h"
#include "UI/ItemSlot.h"
#include "UI/RTInfoPanel.h"
#include "UI/InGame/InGameStore.h"
#include "SlotManager.generated.h"

UCLASS()
class RADIANT_API USlotManager : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<EItemSlotID, class UItemSlot*> Slots;

	UPROPERTY()
	class UUniformGridPanel* InventoryGridPanel;

	UPROPERTY()
	class UHorizontalBox* HotbarHorizontalBox;

public:
	UFUNCTION()
	void OnSlotChanged(const FName& Name, uint32 Amount) const;
	void InitSlots(UHorizontalBox* HorizontalBox, UUniformGridPanel* GridPanel, TSubclassOf<UItemSlot> ItemSlotClass);
	FGameplayTag GetAbilityTrigger(EItemSlotID Slot);

private:
	UItemSlot* GetSlot(const FName& Name) const;
	UItemSlot* GetSlot(EItemSlotID UISlotID) const;
	UItemSlot* FindEmptySlot() const;
	FItemSlotData CreateSlotData(const FName& Name, uint32 Amount) const;
	UItemSlot* CreateNewSlot(const EItemSlotID& UISlotID, TSubclassOf<UItemSlot> ItemSlotClass);
};
