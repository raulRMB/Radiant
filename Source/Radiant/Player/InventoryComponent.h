// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "RTPlayerState.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 Amount;

	UPROPERTY()
	class UAbilityDataAsset* AbilityData;

	FInventoryItem()
	{
		Amount = 0;
		AbilityData = nullptr;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RADIANT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FInventoryItem> Items;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWorldItem> WorldItemClass;

public:
	UInventoryComponent();

private:
	UFUNCTION(Client, Reliable)
	void C_ItemChanged(const FName& ItemName, const uint32 Amount);
	TMap<FName, uint32> InitIngredientList(UCraftingNodeDataAsset* DataAsset);
public:
	UFUNCTION(Server, Reliable)
	void S_DropItem(const FName& ItemName);
	UFUNCTION(Server, Reliable)
	void S_ItemUsed(const FName& ItemName, const uint32 Amount = 1);

	UFUNCTION(Server, Reliable)
	void S_AddItem(const FName& ItemName, const uint32 Amount = 1);
	UFUNCTION(Server, Reliable)
	void S_RemoveAllItems();
	
	int32 AddItem(const FName& ItemName, int32 Amount);
	int32 AddItemUnchecked(const FName& ItemName, int32 Amount);
	bool CheckHasMaterialsToCraft(const TMap<FName, uint32> IngredientList);
	TMap<FName, FInventoryItem> GetItems() const { return Items; }
	void InitInventory(const class UDataTable* ItemDataTable);
	int32 AddItem(const FName& ItemName, FItemData* ItemData, int32 Amount);
	ARTPlayerState* GetPlayerState();
	int32 RemoveItem(const FName& ItemName, int32 Amount);
	void DropItem(const FName& ItemName);
	void UseItem(const FGameplayAbilitySpecHandle& Handle);
	uint32 GetItemAmount(const FName Key) const;
};
