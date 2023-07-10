// Copyright Radiant Studios


#include "UI/InGame/SlotManager/SlotManager.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemData.h"
#include "UI/ItemSlot.h"
#include "Util/Util.h"

void USlotManager::OnSlotChanged(const FName& Name, uint32 Amount) const
{
	UItemSlot* UISlot = GetSlot(Name);
	if(IsValid(UISlot))
	{
		UISlot->SetData(CreateSlotData(Name, Amount));
	}
}

void USlotManager::InitSlots(UHorizontalBox* HorizontalBox, UUniformGridPanel* GridPanel, TSubclassOf<UItemSlot> ItemSlotClass)
{
	HotbarHorizontalBox = HorizontalBox;
	InventoryGridPanel = GridPanel;
	
	for (uint32 i = static_cast<uint32>(EItemSlotID::HotBarFirst); i <= static_cast<uint32>(EItemSlotID::InventoryLast); i++)
	{
		CreateNewSlot(static_cast<EItemSlotID>(i), ItemSlotClass);
	}
}

UItemSlot* USlotManager::GetSlot(const FName& Name) const
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

UItemSlot* USlotManager::GetSlot(EItemSlotID UISlotID) const
{
	if(Slots.Contains(UISlotID))
	{
		return Slots[UISlotID];
	}
	return nullptr;
}

UItemSlot* USlotManager::FindEmptySlot() const
{
	for (uint32 i = static_cast<uint32>(EItemSlotID::HotBarFirst); i <= static_cast<uint32>(EItemSlotID::InventoryLast); i++)
	{
		if(Slots.Contains(static_cast<EItemSlotID>(i)))
		{
			if(Slots[static_cast<EItemSlotID>(i)]->IsEmpty())
			{
				return Slots[static_cast<EItemSlotID>(i)];
			}
		}
	}
	return nullptr;
}

FItemSlotData USlotManager::CreateSlotData(const FName& Name, uint32 Amount) const
{
	FItemSlotData UISlotData;
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

UItemSlot* USlotManager::CreateNewSlot(const EItemSlotID& UISlotID, TSubclassOf<UItemSlot> ItemSlotClass)
{
	UItemSlot* ItemSlot = CreateWidget<UItemSlot>(GetWorld(), ItemSlotClass);
	ItemSlot->SetSlotID(UISlotID);
	Slots.Add(UISlotID, ItemSlot);
	if(HotbarHorizontalBox && InventoryGridPanel)
	{
		if(UISlotID >= EItemSlotID::HotBarFirst && UISlotID <= EItemSlotID::HotBarLast)
		{
			if(UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(HotbarHorizontalBox->AddChild(ItemSlot)))
			{
				Slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				Slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
				Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				Slot->SetPadding({5.f, 0.f, 5.f, 0.f});
			}
		}
		else if(UISlotID >= EItemSlotID::InventoryFirst && UISlotID <= EItemSlotID::InventoryLast)
		{
			if(UUniformGridSlot* Slot = Cast<UUniformGridSlot>(InventoryGridPanel->AddChildToUniformGrid(ItemSlot, InventoryGridPanel->GetChildrenCount() / 5, InventoryGridPanel->GetChildrenCount() % 5)))
			{
				Slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				Slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			}
		}
	}
	return ItemSlot;
}