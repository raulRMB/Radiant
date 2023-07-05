// Copyright Radiant Studios


#include "Items/Inventory.h"

#include "WorldItem.h"
#include "Data/ItemData.h"
#include "Util/Interfaces/Carrier.h"

void UInventory::SetItemClass(TSubclassOf<AWorldItem> NewWorldItemClass)
{
	WorldItemClass = NewWorldItemClass;
}

void UInventory::InitInventory(const UDataTable* ItemDataTable)
{
	TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for(FName RowName : RowNames)
	{
		Items.Add(RowName.ToString(), 0);
	}
	Items["Fireball"] = 1;
}

void UInventory::AddItem(const FString& ItemName)
{
	Items[ItemName] += 1;
}

void UInventory::RemoveItem(const FString& ItemName)
{
	if(Items[ItemName] > 0)
	{
		Items[ItemName] -= 1;
		if(GetWorld() && WorldItemClass)
		{
			if(ICarrier* Carrier = Cast<ICarrier>(GetOuter()))
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
