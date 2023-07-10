// Copyright Radiant Studios


#include "Player/InventoryComponent.h"
#include "Data/ItemData.h"
#include "Engine/DataTable.h"
#include "Event/EventBroker.h"
#include "Items/WorldItem.h"
#include "Util/Interfaces/Carrier.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::S_ItemUsed_Implementation(const FName& ItemName)
{
	if(GetOwner()->HasAuthority())
	{
		if(Items.Contains(ItemName))
		{
			if(Items[ItemName].Amount > 0)
			{
				RemoveItem(ItemName);
			}
		}
	}
}

void UInventoryComponent::C_ItemChanged_Implementation(const FName& ItemName, const uint32 Amount)
{
	UEventBroker::Get(this)->ItemChanged.Broadcast(ItemName, Amount);
}

void UInventoryComponent::S_DropItem_Implementation(const FName& ItemName)
{
	if(GetOwner()->HasAuthority())
	{
		DropItem(ItemName);
	}
}

void UInventoryComponent::InitInventory(const UDataTable* ItemDataTable)
{
	TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for(FName RowName : RowNames)
	{
		if(FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, "InitInventory"))
		{
			FInventoryItem InventoryItem;
			InventoryItem.Amount = 0;
			InventoryItem.AbilityData = ItemData->AbilityData;
			Items.Add(RowName, InventoryItem);
		}
	}
	UEventBroker::Get(this)->ItemUsed.AddUObject(this, &UInventoryComponent::S_ItemUsed);
}

int32 UInventoryComponent::AddItem(const FName& ItemName)
{
	Items[ItemName].Amount += 1;
	C_ItemChanged(ItemName, Items[ItemName].Amount);
	return Items[ItemName].Amount;
}

int32 UInventoryComponent::RemoveItem(const FName& ItemName)
{
	if(Items[ItemName].Amount > 0)
	{
		Items[ItemName].Amount  -= 1;
		C_ItemChanged(ItemName, Items[ItemName].Amount);
	}
	return Items[ItemName].Amount;
}

void UInventoryComponent::DropItem(const FName& ItemName)
{
	if(GetWorld() && WorldItemClass)
	{
		if(ICarrier* Carrier = Cast<ICarrier>(GetOwner()))
		{
			FVector Offset = FVector::RightVector;
			Offset.RotateAngleAxis(FMath::RandRange(0, 360), FVector::UpVector);
			Offset *= 100;
			FTransform SpawnTransform = FTransform(Carrier->GetCarrierLocation() + Offset);
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
	if(HandleToItemName.Contains(Handle))
	{
		S_ItemUsed(HandleToItemName[Handle]);
	}
}

void UInventoryComponent::AddHandleToName(FGameplayAbilitySpecHandle Handle, FName Name)
{
	HandleToItemName.Add(Handle, Name);
}
