// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Util/Enums/InventorySlot.h"
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
	
	UPROPERTY(ReplicatedUsing=OnRepCurrentAbilities, EditAnywhere)
	TArray<class UAbilityDataAsset*> CurrentAbilities;

	TMap<EInventorySlot, UAbilityDataAsset*> HotBarAbilities;

public:	
	UInventoryComponent();

private:
	UFUNCTION()
	void OnRepCurrentAbilities(TArray<UAbilityDataAsset*> OldAbilities);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:

	TMap<FName, FInventoryItem> GetItems() const { return Items; }
	void InitInventory(const class UDataTable* ItemDataTable);
	void AddItem(const FName& ItemName);
	void RemoveItem(const FName& ItemName);
	void SwapHotbarSlot(EInventorySlot One, EInventorySlot Two);

	struct FGameplayTag GetAbilityTrigger(uint32 i) const;
	const TMap<EInventorySlot, UAbilityDataAsset*>& GetHotBarAbilities() const { return HotBarAbilities; }
	TArray<UAbilityDataAsset*> GetCurrentAbilities() const;
};
