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
}

void UInventoryComponent::AddItem(const FName& ItemName)
{
	Items[ItemName].Amount += 1;
	C_AddItem(Items[ItemName]);
}

void UInventoryComponent::RemoveItem(const FName& ItemName)
{
	if(Items[ItemName].Amount > 0)
	{
		Items[ItemName].Amount  -= 1;
		if(Items[ItemName].Amount == 0)
		{
			Cast<ICarrier>(GetOwner())->DropItem(ItemName);
		}
		if(GetWorld() && WorldItemClass)
		{
			if(ICarrier* Carrier = Cast<ICarrier>(GetOwner()))
			{
				FVector Offset = FVector::RightVector;
				Offset.RotateAngleAxis(FMath::RandRange(0, 360), FVector::UpVector);
				Offset *= 100;
				FTransform SpawnTransform = FTransform(Carrier->GetCarrierLocation() + Offset);
				AWorldItem* WorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(WorldItemClass, SpawnTransform);
				WorldItem->InitItem(ItemName);
				WorldItem->FinishSpawning(SpawnTransform);
			}
		}
	}
}

void UInventoryComponent::C_AddItem_Implementation(const FInventoryItem& Item)
{
	UEventBroker::Get(this)->ItemChanged.Broadcast(Item);
}


