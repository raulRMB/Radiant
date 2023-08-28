// Copyright Radiant Studios


#include "Player/InventoryComponent.h"

#include "RTPlayerState.h"
#include "Data/CraftingNodeDataAsset.h"
#include "Data/ItemData.h"
#include "Engine/DataTable.h"
#include "Event/EventBroker.h"
#include "Items/WorldItem.h"
#include "Util/Util.h"
#include "Util/Interfaces/Carrier.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::C_ItemChanged_Implementation(const FName& ItemName, const uint32 Amount)
{
	if(Items.Contains(ItemName))
	{
		Items[ItemName].Amount = Amount;
	}
	
	UEventBroker::Get(this)->ItemChanged.Broadcast(ItemName, Amount);
}

void UInventoryComponent::S_DropItem_Implementation(const FName& ItemName)
{
	if (GetOwner()->HasAuthority())
	{
		DropItem(ItemName);
	}
}

void UInventoryComponent::S_ItemUsed_Implementation(const FName& ItemName, const uint32 Amount)
{
	if (GetOwner()->HasAuthority())
	{
		if (Items.Contains(ItemName))
		{
			if (Items[ItemName].Amount > 0)
			{
				RemoveItem(ItemName, Amount);
			}
		}
	}
}

void UInventoryComponent::S_AddItem_Implementation(const FName& ItemName, const uint32 Amount)
{
	AddItem(ItemName, Amount);
}

void UInventoryComponent::S_RemoveAllItems_Implementation()
{
	for(TPair<FName, FInventoryItem>& Item : Items)
	{
		Item.Value.Amount = 0;
		C_ItemChanged(Item.Key, Item.Value.Amount);
	}
}

void UInventoryComponent::InitInventory(const UDataTable* ItemDataTable)
{
	TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for (FName RowName : RowNames)
	{
		if (FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, "InitInventory"))
		{
			if(!ItemData->Enabled)
			{
				continue;
			}
			FInventoryItem InventoryItem;
			InventoryItem.Amount = 0;
			InventoryItem.AbilityData = ItemData->AbilityData;
			Items.Add(RowName, InventoryItem);
		}
	}
	UEventBroker::Get(this)->ItemUsed.AddUObject(this, &UInventoryComponent::S_ItemUsed);
}

int32 UInventoryComponent::AddItem(const FName& ItemName, FItemData* ItemData, int32 Amount)
{
	return AddItem(ItemName, Amount);
}

int32 UInventoryComponent::AddItem(const FName& ItemName, int32 Amount)
{
	if(FItemData* ItemData = UUtil::GetItemDataFromName(ItemName))
	{
		const auto IngredientList = InitIngredientList(ItemData->CraftingNodeData);
		if(CheckHasMaterialsToCraft(IngredientList))
		{
			for(auto Ingredient : IngredientList)
			{
				S_ItemUsed(Ingredient.Key, Ingredient.Value);
			}
			return AddItemUnchecked(ItemName, Amount);
		}
	}
	return 0;
}

int32 UInventoryComponent::AddItemUnchecked(const FName& ItemName, int32 Amount)
{
	if(Items.Contains(ItemName))
	{
		Items[ItemName].Amount += Amount;
		C_ItemChanged(ItemName, Items[ItemName].Amount);
		UE_LOG(LogTemp, Warning, TEXT("Added %d %s total: %d"), Amount, *ItemName.ToString(), Items[ItemName].Amount);
		return Items[ItemName].Amount;
	}
	return 0;
}

bool UInventoryComponent::CheckHasMaterialsToCraft(const TMap<FName, uint32> IngredientList)
{
	bool bHasMaterials = true;
	for(TPair<FName, uint32> Ingredient : IngredientList)
	{
		auto val = GetItemAmount(Ingredient.Key);
		if(Ingredient.Value > val)
		{
			bHasMaterials = false;
			break;
		}
	}
	return bHasMaterials;
}

TMap<FName, uint32> UInventoryComponent::InitIngredientList(UCraftingNodeDataAsset* DataAsset)
{
	TMap<FName, uint32> IngredientList = TMap<FName, uint32>();
	for(TPair<UCraftingNodeDataAsset*, uint32> Material : DataAsset->Materials)
	{
		if(IngredientList.Contains(Material.Key->Name))
		{
			IngredientList[Material.Key->Name] += Material.Value;
			continue;
		}
		IngredientList.Add(Material.Key->Name, Material.Value);
	}
	return IngredientList;
}

ARTPlayerState* UInventoryComponent::GetPlayerState()
{
	return Cast<ARTPlayerController>(GetOuter())->GetPlayerState<ARTPlayerState>();
}

int32 UInventoryComponent::RemoveItem(const FName& ItemName, int32 Amount)
{
	if (Items[ItemName].Amount > 0)
	{
		Items[ItemName].Amount -= Amount;
		C_ItemChanged(ItemName, Items[ItemName].Amount);
	}
	if(Amount == -1)
	{
		Items[ItemName].Amount = 0;
		C_ItemChanged(ItemName, Items[ItemName].Amount);
	}
	
	return Items[ItemName].Amount;
}

void UInventoryComponent::DropItem(const FName& ItemName)
{
	if (GetWorld() && WorldItemClass)
	{
		if (ICarrier* Carrier = Cast<ICarrier>(GetOwner()))
		{
			FVector Offset = FVector::RightVector;
			Offset = Offset.RotateAngleAxis(FMath::RandRange(0.0f, 360.0f), FVector::UpVector);
			Offset *= 100.0f;
			FTransform SpawnTransform = FTransform((Carrier->GetCarrierLocation() + Offset) * FVector(1.f,1.f,0.f));
			AWorldItem* WorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(WorldItemClass, SpawnTransform);
			WorldItem->FinishSpawning(SpawnTransform);
			WorldItem->InitItem(ItemName, Items[ItemName].Amount);
		}
	}
	Items[ItemName].Amount = 0;
	C_ItemChanged(ItemName, Items[ItemName].Amount);
}

void UInventoryComponent::UseItem(const FGameplayAbilitySpecHandle& Handle)
{
	auto HandleToItemName = GetPlayerState()->HandleToItemName;
	if (HandleToItemName.Contains(Handle))
	{
		S_ItemUsed(HandleToItemName[Handle]);
	}
}

uint32 UInventoryComponent::GetItemAmount(const FName Key) const
{
	if(Items.Num() > 0)
	{
		if(Items.Contains(Key))
		{
			return Items[Key].Amount;
		}
	}
	return 0;
}
