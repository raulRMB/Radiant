// Copyright Radiant Studios


#include "UI/InGame/SlotManager/SlotManager.h"

#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Data/GearData.h"
#include "Data/ItemData.h"
#include "Event/EventBroker.h"
#include "UI/GearSlot.h"
#include "UI/ItemSlot.h"
#include "UI/Crafting/CraftingPanel.h"
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

void USlotManager::OnClassChanged(EClassType Class)
{
	for(int i = static_cast<int>(EItemSlotID::HotBarFirst); i <= static_cast<int>(EItemSlotID::HotBarLast); i++)
	{
		auto Slot = Slots[static_cast<EItemSlotID>(i)];
		if(Slot->IsEmpty() || Slot->GetItemClass() == EClassType::General)
		{
			continue;
		}
		if(Slot->GetItemClass() != Class)
		{
			Slot->SwapWith(FindEmptySlot());
		}
	}
}

void USlotManager::InitSlots(URTInfoPanel* InfoPanel, UCraftingPanel* CraftingPanel, TSubclassOf<UItemSlot> ItemSlotClass)
{
	UEventBroker::Get(this)->ItemChanged.AddUObject(this, &USlotManager::OnSlotChanged);
	UEventBroker::Get(this)->CurrentClassChanged.AddUObject(this, &USlotManager::OnClassChanged);
	HotbarHorizontalBox = InfoPanel->GetHotbarHorizontalBox();
	InventoryGridPanel = CraftingPanel->GetInventoryGrid();
	
	WeaponSlot = InfoPanel->GetWeaponSlot();
	
	HelmetSlot = CraftingPanel->GetHelmetSlot();
	ChestSlot = CraftingPanel->GetChestSlot();
	LegsSlot = CraftingPanel->GetLegsSlot();
	BootsSlot = CraftingPanel->GetBootsSlot();
	GlovesSlot = CraftingPanel->GetGlovesSlot();
	JewelrySlot = CraftingPanel->GetJewelrySlot();
	
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
	auto ItemData = UUtil::GetItemDataFromName(Name);
	if(ItemData->GearData)
	{
		if(ItemData->GearData->GearType == EGearType::Weapon && Slots[EItemSlotID::WeaponSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::WeaponSlot];
		}
		if(ItemData->GearData->GearType == EGearType::Boots && Slots[EItemSlotID::BootsSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::BootsSlot];
		}
		if(ItemData->GearData->GearType == EGearType::Chest && Slots[EItemSlotID::ChestSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::ChestSlot];
		}
		if(ItemData->GearData->GearType == EGearType::Gloves && Slots[EItemSlotID::GlovesSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::GlovesSlot];
		}
		if(ItemData->GearData->GearType == EGearType::Helmet && Slots[EItemSlotID::HelmetSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::HelmetSlot];
		}
		if(ItemData->GearData->GearType == EGearType::Legs && Slots[EItemSlotID::LegsSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::LegsSlot];
		}
		if(ItemData->GearData->GearType == EGearType::Necklace && Slots[EItemSlotID::NecklaceSlot]->IsEmpty())
		{
			return Slots[EItemSlotID::NecklaceSlot];
		}
	}
	if(ItemData->ItemType == EItemType::Ability && Slots[EItemSlotID::WeaponSlot]->GetItemClass() == ItemData->ClassType)
	{
		return FindEmptyAbilitySlot();
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
	for (uint32 i = static_cast<uint32>(EItemSlotID::InventoryFirst); i <= static_cast<uint32>(EItemSlotID::InventoryLast); i++)
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

UItemSlot* USlotManager::FindEmptyAbilitySlot() const
{
	for (uint32 i = static_cast<uint32>(EItemSlotID::HotBarFirst); i <= static_cast<uint32>(EItemSlotID::HotBarLast); i++)
	{
		if(Slots.Contains(static_cast<EItemSlotID>(i)))
		{
			if(Slots[static_cast<EItemSlotID>(i)]->IsEmpty())
			{
				return Slots[static_cast<EItemSlotID>(i)];
			}
		}
	}
	return FindEmptySlot();
}

FItemSlotData USlotManager::CreateSlotData(const FName& Name, uint32 Amount) const
{
	FItemSlotData ItemSlotData;
	FItemData* ItemData = UUtil::GetItemDataFromName(Name, FString("SlotManager::OnSlotChanged"));		
	if(ItemData)
	{
		ItemSlotData.ItemName = Name;
		ItemSlotData.ItemAmount = Amount;
		ItemSlotData.ItemType = ItemData->ItemType;
		ItemSlotData.ClassType = ItemData->ClassType;
		if(ItemData->AbilityData)
		{
			ItemSlotData.AbilityTrigger = ItemData->AbilityData->Ability.GetDefaultObject()->GetTriggerTag();
			ItemSlotData.CooldownTag = ItemData->AbilityData->Ability.GetDefaultObject()->GetCooldownTag();
			ItemSlotData.Icon = ItemData->Icon;
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

			InventoryGridPanel->AddChildToUniformGrid(SizeBox, InventoryGridPanel->GetChildrenCount() / 9, InventoryGridPanel->GetChildrenCount() % 9);
		}
		else if(UISlotID == EItemSlotID::WeaponSlot)
		{
			WeaponSlot->SetEmpty(true);
			WeaponSlot->SetSlotID(UISlotID);
			Slots[UISlotID] = WeaponSlot;
			return WeaponSlot;
		}
		else if(UISlotID == EItemSlotID::HelmetSlot)
		{
			HelmetSlot->SetEmpty(true);
			HelmetSlot->SetSlotID(EItemSlotID::HelmetSlot);
			Slots[EItemSlotID::HelmetSlot] = HelmetSlot;				
			return HelmetSlot;
		}
		else if(UISlotID == EItemSlotID::ChestSlot)
		{
			ChestSlot->SetEmpty(true);
			ChestSlot->SetSlotID(EItemSlotID::ChestSlot);
			Slots[EItemSlotID::ChestSlot] = ChestSlot;				
			return ChestSlot;
		}
		else if(UISlotID == EItemSlotID::LegsSlot)
		{
			LegsSlot->SetEmpty(true);
			LegsSlot->SetSlotID(EItemSlotID::LegsSlot);
			Slots[EItemSlotID::LegsSlot] = LegsSlot;				
			return LegsSlot;
		}
		else if(UISlotID == EItemSlotID::NecklaceSlot)
		{
			JewelrySlot->SetEmpty(true);
			JewelrySlot->SetSlotID(EItemSlotID::NecklaceSlot);
			Slots[EItemSlotID::NecklaceSlot] = JewelrySlot;
			return JewelrySlot;
		}
		else if(UISlotID == EItemSlotID::GlovesSlot)
		{
			GlovesSlot->SetEmpty(true);
			GlovesSlot->SetSlotID(EItemSlotID::GlovesSlot);
			Slots[EItemSlotID::GlovesSlot] = GlovesSlot;
			return GlovesSlot;
		}
		else if(UISlotID == EItemSlotID::BootsSlot)
		{
			BootsSlot->SetEmpty(true);
			BootsSlot->SetSlotID(EItemSlotID::BootsSlot);
			Slots[EItemSlotID::BootsSlot] = BootsSlot;
			return BootsSlot;
		}
	}
	return ItemSlot;
}