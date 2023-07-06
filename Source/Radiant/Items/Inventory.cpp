// Copyright Radiant Studios


#include "Items/Inventory.h"

#include "WorldItem.h"
#include "Data/ItemData.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"
#include "Util/Enums/InventorySlot.h"
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
		if(FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, "InitInventory"))
		{
			FInventoryItem InventoryItem;
			InventoryItem.Amount = 0;
			InventoryItem.AbilityData = ItemData->AbilityData;
			Items.Add(RowName, InventoryItem);
		}
	}
}

void UInventory::AddItem(const FName& ItemName)
{
	Items[ItemName].Amount += 1;
	CurrentAbilities.Add(Items[ItemName].AbilityData);
}

void UInventory::RemoveItem(const FName& ItemName)
{	
	if(Items[ItemName].Amount > 0)
	{
		Items[ItemName].Amount  -= 1;
		if(Items[ItemName].Amount == 0)
		{
			CurrentAbilities.Remove(Items[ItemName].AbilityData);
			Cast<ICarrier>(GetOuter())->DropItem(ItemName);
		}
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

void UInventory::OnRepCurrentAbilities(TArray<UAbilityDataAsset*> OldPurchasedAbilities)
{
	ARTPlayerState* PS = Cast<ARTPlayerState>(GetOuter());
	
	if(PS)
	{
		for (int i = 0; i < CurrentAbilities.Num(); i++)
		{
			if(!OldPurchasedAbilities.Contains(CurrentAbilities[i]))
			{
				for (int j = 0; j <= static_cast<uint32>(EInventorySlot::Six); j++)
				{
					EInventorySlot slot = static_cast<EInventorySlot>(j);
					if(!HotBarAbilities.Contains(slot))
					{
						HotBarAbilities.Add(slot, CurrentAbilities[i]);
						break;
					}
				}
			}
		}
		
		PS->GetPawn<AAvatar>()->SetHUDIcons(HotBarAbilities);
	}
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{	
	DOREPLIFETIME(UInventory, CurrentAbilities);
}

FGameplayTag UInventory::GetAbilityTrigger(const uint32 i) const
{
	auto Ability = HotBarAbilities.FindRef(static_cast<EInventorySlot>(i));
	if(Ability)
	{
		return Ability->Ability.GetDefaultObject()->GetTriggerTag();
	}
	return FGameplayTag();
}

void UInventory::SwapHotbarSlot(EInventorySlot One, EInventorySlot Two)
{
	if(HotBarAbilities.Contains(One) && HotBarAbilities.Contains(Two))
	{
		UAbilityDataAsset* Temp = HotBarAbilities[One];
		HotBarAbilities[One] = HotBarAbilities[Two];
		HotBarAbilities[Two] = Temp;
	} else if(HotBarAbilities.Contains(One))
	{
		HotBarAbilities.Add(Two, HotBarAbilities[One]);
		HotBarAbilities.Remove(One);
	} else if(HotBarAbilities.Contains(Two))
	{
		HotBarAbilities.Add(One, HotBarAbilities[Two]);
		HotBarAbilities.Remove(Two);
	}
}

TArray<UAbilityDataAsset*> UInventory::GetCurrentAbilities() const
{
	return CurrentAbilities;
}
