// Copyright Radiant Studios


#include "Player/InventoryComponent.h"
#include "Avatar.h"
#include "RTPlayerController.h"
#include "Data/ItemData.h"
#include "Engine/DataTable.h"
#include "Items/WorldItem.h"
#include "Net/UnrealNetwork.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"
#include "Util/Interfaces/Carrier.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::OnRepCurrentAbilities(TArray<UAbilityDataAsset*> OldAbilities)
{
	for (int i = 0; i < CurrentAbilities.Num(); i++)
	{
		if(!OldAbilities.Contains(CurrentAbilities[i]))
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

	if(ARTPlayerController* PC = Cast<ARTPlayerController>(GetOwner()))
	{
		PC->GetPawn<AAvatar>()->SetHUDIcons(HotBarAbilities);
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
}

void UInventoryComponent::AddItem(const FName& ItemName)
{
	if(Items[ItemName].Amount == 0)
	{
		CurrentAbilities.Add(Items[ItemName].AbilityData);
	}
	Items[ItemName].Amount += 1;
}

void UInventoryComponent::RemoveItem(const FName& ItemName)
{
	if(Items[ItemName].Amount > 0)
	{
		Items[ItemName].Amount  -= 1;
		if(Items[ItemName].Amount == 0)
		{
			CurrentAbilities.Remove(Items[ItemName].AbilityData);
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

void UInventoryComponent::SwapHotbarSlot(EInventorySlot One, EInventorySlot Two)
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

FGameplayTag UInventoryComponent::GetAbilityTrigger(uint32 i) const
{
	auto Ability = HotBarAbilities.FindRef(static_cast<EInventorySlot>(i));
	if(Ability)
	{
		return Ability->Ability.GetDefaultObject()->GetTriggerTag();
	}
	return FGameplayTag();
}

TArray<UAbilityDataAsset*> UInventoryComponent::GetCurrentAbilities() const
{
	return CurrentAbilities;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, CurrentAbilities);
}

