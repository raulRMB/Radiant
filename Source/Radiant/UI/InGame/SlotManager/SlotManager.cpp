// Copyright Radiant Studios


#include "UI/InGame/SlotManager/SlotManager.h"

#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemData.h"
#include "Event/EventBroker.h"
#include "UI/ItemSlot.h"
#include "UI/WeaponSlot.h"
#include "Util/Util.h"

void USlotManager::OnSlotChanged(const FName& Name, uint32 Amount) const
{
	UItemSlot* ItemSlot = GetSlot(Name);
	if(IsValid(ItemSlot))
	{
		if(Amount == 0)
		{
			ItemSlot->Reset();
			return;
		}
			
		ItemSlot->SetData(CreateSlotData(Name, Amount));
		ItemSlot->SetEmpty(false);
	}
}

void USlotManager::InitSlots(URTInfoPanel* InfoPanel, UUniformGridPanel* GridPanel, TSubclassOf<UItemSlot> ItemSlotClass, UUniformGridPanel* InGearGridPanel)
{
	UEventBroker::Get(this)->ItemChanged.AddUObject(this, &USlotManager::OnSlotChanged);
	
	HotbarHorizontalBox = InfoPanel->GetHotbarHorizontalBox();
	InventoryGridPanel = GridPanel;
	WeaponSlot = InfoPanel->GetWeaponSlot();
	GearGridPanel = InGearGridPanel;
	
	for (uint32 i = static_cast<uint32>(EItemSlotID::HotBarFirst); i <= static_cast<uint32>(EItemSlotID::NecklaceSlot); i++)
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
		ItemSlotData.bIsWeapon = ItemData->bIsWeapon;
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
	if(HotbarHorizontalBox && InventoryGridPanel && WeaponSlot)
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
			USizeBox* SizeBox = ItemSlot->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
			SizeBox->SetWidthOverride(100.f);
			SizeBox->SetHeightOverride(100.f);
			SizeBox->AddChild(ItemSlot);

			InventoryGridPanel->AddChildToUniformGrid(SizeBox, InventoryGridPanel->GetChildrenCount() / 11, InventoryGridPanel->GetChildrenCount() % 11);
		}
		else if(UISlotID == EItemSlotID::WeaponSlot)
		{
			WeaponSlot->SetEmpty(true);
			WeaponSlot->SetSlotID(UISlotID);
			Slots[UISlotID] = WeaponSlot;
			return WeaponSlot;
		}
		else if(UISlotID >= EItemSlotID::HelmetSlot && UISlotID <= EItemSlotID::NecklaceSlot)
		{
			USizeBox* SizeBox = ItemSlot->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
			SizeBox->SetWidthOverride(100.f);
			SizeBox->SetHeightOverride(100.f);
			SizeBox->AddChild(ItemSlot);
			
			GearGridPanel->AddChildToUniformGrid(SizeBox, GearGridPanel->GetChildrenCount() / 3, GearGridPanel->GetChildrenCount() % 3);
		}
	}
	return ItemSlot;
}