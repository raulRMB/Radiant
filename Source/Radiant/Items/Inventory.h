// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Inventory.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UInventory : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, uint16> Items;

	UPROPERTY()
	TSubclassOf<class AWorldItem> WorldItemClass;
public:
	TMap<FString, uint16> GetItems() const { return Items; }	
	void SetItemClass(TSubclassOf<class AWorldItem> NewWorldItemClass);
	void InitInventory(const class UDataTable* ItemDataTable);
	void AddItem(const FString& ItemName);
	void RemoveItem(const FString& ItemName);
};
