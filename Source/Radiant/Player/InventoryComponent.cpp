// Copyright Radiant Studios


#include "Player/InventoryComponent.h"

#include "RTPlayerState.h"
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

void UInventoryComponent::InitInventory(const UDataTable* ItemDataTable)
{
	TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for (FName RowName : RowNames)
	{
		if (FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, "InitInventory"))
		{
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
	if (ItemData->AbilityData && !FindHandle(ItemName))
	{
		FGameplayAbilitySpecHandle Handle = GetPlayerState()->GiveAbility(ItemData);
		AddHandleToName(Handle, ItemName);
	}
	return AddItem(ItemName, Amount);
}

int32 UInventoryComponent::AddItem(const FName& ItemName, int32 Amount)
{
	if(Items.Contains(ItemName))
	{
		Items[ItemName].Amount += Amount;
		C_ItemChanged(ItemName, Items[ItemName].Amount);
		return Items[ItemName].Amount;
	}
	return 0;
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
	if(Items[ItemName].Amount == 0)
	{
		if(const auto Handle = FindHandle(ItemName))
		{
			GetPlayerState()->RemoveAbility(*Handle);
			HandleToItemName.Remove(*Handle);
		}
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
	if (HandleToItemName.Contains(Handle))
	{
		S_ItemUsed(HandleToItemName[Handle]);
	}
}

void UInventoryComponent::AddHandleToName(FGameplayAbilitySpecHandle Handle, FName Name)
{
	HandleToItemName.Add(Handle, Name);
}

const FGameplayAbilitySpecHandle* UInventoryComponent::FindHandle(FName Name)
{
	return HandleToItemName.FindKey(Name);
}

FName UInventoryComponent::GetItemNameFormHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (HandleToItemName.Contains(Handle))
	{
		return HandleToItemName[Handle];
	}
	return NAME_None;
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
