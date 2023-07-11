// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RADIANT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FInventoryItem> Items;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWorldItem> WorldItemClass;

	UPROPERTY()
	TMap<FGameplayAbilitySpecHandle, FName> HandleToItemName;
public:	
	UInventoryComponent();
private:
	UFUNCTION(Client, Reliable)
	void C_ItemChanged(const FName& ItemName, const uint32 Amount);
public:
	UFUNCTION(Server, Reliable)
	void S_DropItem(const FName& ItemName);
	UFUNCTION(Server, Reliable)
	void S_ItemUsed(const FName& ItemName);
public:
	TMap<FName, FInventoryItem> GetItems() const { return Items; }
	void InitInventory(const class UDataTable* ItemDataTable);
	int32 AddItem(const FName& ItemName);
	int32 RemoveItem(const FName& ItemName);
	void DropItem(const FName& ItemName);
	void UseItem(const FGameplayAbilitySpecHandle& Handle);
	void AddHandleToName(FGameplayAbilitySpecHandle Handle, FName Name);

	FName GetItemNameFormHandle(const FGameplayAbilitySpecHandle& Handle);
};
