// Copyright Radiant Studios


#include "UI/InGame/SlotManager/SlotManager.h"

#include "Data/ItemData.h"
#include "UI/UISlot.h"
#include "Util/Util.h"

void USlotManager::OnSlotChanged(const FName& Name, uint32 Amount) const
{
	UUISlot* UISlot = GetSlot(Name);
	if(IsValid(UISlot))
	{
		UISlot->SetData(CreateSlotData(Name, Amount));
	}
}

UUISlot* USlotManager::GetSlot(const FName& Name) const
{
	for(auto& Pair : Slots)
	{
		if(Pair.Value->GetItemName() == Name)
		{
			return Pair.Value;
		}
	}
	return FindEmptySlot();
}

UUISlot* USlotManager::GetSlot(EUISlotID UISlotID) const
{
	if(Slots.Contains(UISlotID))
	{
		return Slots[UISlotID];
	}
	return nullptr;
}

UUISlot* USlotManager::FindEmptySlot() const
{
	for (uint32 i = static_cast<uint32>(EUISlotID::HotBarFirst); i <= static_cast<uint32>(EUISlotID::InventoryLast); i++)
	{
		if(Slots.Contains(static_cast<EUISlotID>(i)))
		{
			if(Slots[static_cast<EUISlotID>(i)]->IsEmpty())
			{
				return Slots[static_cast<EUISlotID>(i)];
			}
		}
	}
	return nullptr;
}

FUISlotData USlotManager::CreateSlotData(const FName& Name, uint32 Amount) const
{
	FUISlotData UISlotData;
	FItemData* ItemData = UUtil::GetItemDataFromName(Name, FString("SlotManager::OnSlotChanged"));		
	if(ItemData)
	{
		UISlotData.ItemName = Name;
		UISlotData.ItemAmount = Amount;
		if(ItemData->AbilityData)
		{
			UISlotData.AbilityTrigger = ItemData->AbilityData->Ability.GetDefaultObject()->GetTriggerTag();
			UISlotData.CooldownTag = ItemData->AbilityData->Ability.GetDefaultObject()->GetCooldownTag();
			UISlotData.Tooltip = ItemData->AbilityData->Tooltip;
		}
		else
		{
			UISlotData.Tooltip = ItemData->Tooltip;
		}
	}
	return UISlotData;
}
