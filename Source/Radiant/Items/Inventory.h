// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Util/Enums/InventorySlot.h"
#include "Util/Replication/ReplicatedObject.h"
#include "Inventory.generated.h"

USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	uint16 Amount;
	UPROPERTY()
	class UAbilityDataAsset* AbilityData;
};


UCLASS()
class RADIANT_API UInventory : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FInventoryItem> Items;

	UPROPERTY()
	TSubclassOf<class AWorldItem> WorldItemClass;
	
	UPROPERTY(ReplicatedUsing=OnRepCurrentAbilities, EditAnywhere)
	TArray<class UAbilityDataAsset*> CurrentAbilities;
	
	UPROPERTY()
	TMap<EInventorySlot, UAbilityDataAsset*> HotBarAbilities;
private:
	UFUNCTION()
	void OnRepCurrentAbilities(TArray<UAbilityDataAsset*> OldPurchasedAbilities);
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	TMap<FName, FInventoryItem> GetItems() const { return Items; }
	void SetItemClass(TSubclassOf<class AWorldItem> NewWorldItemClass);
	void InitInventory(const class UDataTable* ItemDataTable);
	void AddItem(const FName& ItemName);
	void RemoveItem(const FName& ItemName);
	void SwapHotbarSlot(EInventorySlot One, EInventorySlot Two);
	
	virtual bool IsSupportedForNetworking() const override { return true; }

public:
	FGameplayTag GetAbilityTrigger(uint32 i) const;
	const TMap<EInventorySlot, UAbilityDataAsset*>& GetHotBarAbilities() const { return HotBarAbilities; }
	TArray<UAbilityDataAsset*> GetCurrentAbilities() const;
};
