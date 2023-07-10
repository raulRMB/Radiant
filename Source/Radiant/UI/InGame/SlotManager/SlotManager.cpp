// Copyright Radiant Studios


#include "UI/InGame/SlotManager/SlotManager.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemData.h"
#include "Event/EventBroker.h"
#include "UI/ItemSlot.h"
#include "Util/Util.h"

void USlotManager::OnSlotChanged(const FName& Name, uint32 Amount) const
{
	UItemSlot* ItemSlot = GetSlot(Name);
	if(IsValid(ItemSlot))
	{
		ItemSlot->SetData(CreateSlotData(Name, Amount));
		ItemSlot->SetEmpty(false);
	}
}

void USlotManager::InitSlots(UHorizontalBox* HorizontalBox, UUniformGridPanel* GridPanel, TSubclassOf<UItemSlot> ItemSlotClass)
{
	UEventBroker::Get(this)->ItemChanged.AddUObject(this, &USlotManager::OnSlotChanged);
	
	HotbarHorizontalBox = HorizontalBox;
	InventoryGridPanel = GridPanel;
	
	for (uint32 i = static_cast<uint32>(EItemSlotID::HotBarFirst); i <= static_cast<uint32>(EItemSlotID::InventoryLast); i++)
	{
		CreateNewSlot(static_cast<EItemSlotID>(i), ItemSlotClass);
	}
}

FGameplayTag USlotManager::GetAbilityTrigger(EItemSlotID Slot)
{
	if(Slots.Contains(Slot))
	{
		return Slots[Slot]->GetAbilityTrigger();
	}
	return FGameplayTag();
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
	FItemSlotData ItemSlotData;
	FItemData* ItemData = UUtil::GetItemDataFromName(Name, FString("SlotManager::OnSlotChanged"));		
	if(ItemData)
	{
		ItemSlotData.ItemName = Name;
		ItemSlotData.ItemAmount = Amount;
		if(ItemData->AbilityData)
		{
			ItemSlotData.AbilityTrigger = ItemData->AbilityData->Ability.GetDefaultObject()->GetTriggerTag();
			ItemSlotData.CooldownTag = ItemData->AbilityData->Ability.GetDefaultObject()->GetCooldownTag();
			ItemSlotData.Tooltip = ItemData->AbilityData->Tooltip;
			ItemSlotData.Icon = ItemData->AbilityData->Icon;
		}
		else
		{
			ItemSlotData.Tooltip = ItemData->Tooltip;
			ItemSlotData.Icon = ItemData->Icon;
		}
	}
	return ItemSlotData;
}

UItemSlot* USlotManager::CreateNewSlot(const EItemSlotID& UISlotID, TSubclassOf<UItemSlot> ItemSlotClass)
{
	UItemSlot* ItemSlot = CreateWidget<UItemSlot>(GetWorld(), ItemSlotClass);
	ItemSlot->SetEmpty(true);
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