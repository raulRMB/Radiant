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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWorldItem> WorldItemClass;
public:	
	UInventoryComponent();
private:
	UFUNCTION(Client, Reliable)
	void C_ItemChanged(const FInventoryItem& Item);
public:
	UFUNCTION(Server, Reliable)
	void S_DropItem(const FName& ItemName);
	
public:
	TMap<FName, FInventoryItem> GetItems() const { return Items; }
	void InitInventory(const class UDataTable* ItemDataTable);
	void AddItem(const FName& ItemName);
	void RemoveItem(const FName& ItemName);
	void DropItem(const FName& ItemName);
};
