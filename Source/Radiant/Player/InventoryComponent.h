// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	uint16 Amount;
	UPROPERTY()
	class UAbilityDataAsset* AbilityData;

	FInventoryItem()
	{
		Amount = 0;
		AbilityData = nullptr;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RADIANT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	TMap<FName, FInventoryItem> Items;

	UPROPERTY()
	TSubclassOf<class AWorldItem> WorldItemClass;
public:	
	UInventoryComponent();
private:
	UFUNCTION(Client, Reliable)
	void C_AddItem(const FInventoryItem& Item);
public:
	TMap<FName, FInventoryItem> GetItems() const { return Items; }
	void InitInventory(const class UDataTable* ItemDataTable);
	void AddItem(const FName& ItemName);
	void RemoveItem(const FName& ItemName);
};
