﻿#include "GearSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/GearData.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UGearSlot::OnAfterFill()
{
	Super::OnAfterFill();
	GetOwningPlayerState<ARTPlayerState>()->S_EquipGear(ItemSlotData.ItemName);
}

void UGearSlot::OnBeforeEmpty()
{
	Super::OnBeforeEmpty();
	GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
}

void UGearSlot::OnBeforeSwap()
{
	Super::OnBeforeSwap();
	GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
}

void UGearSlot::OnBeforeItemDropped()
{
	Super::OnBeforeItemDropped();
	GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
}

void UGearSlot::OnAfterSwap()
{
	Super::OnAfterSwap();
	GetOwningPlayerState<ARTPlayerState>()->S_EquipGear(ItemSlotData.ItemName);
}

bool UGearSlot::CheckCanSwapWith(UItemSlot* ItemSlot)
{
	if(ItemSlot == nullptr)
		return false;

	if(ItemSlot->IsEmpty())
	{
		return true;
	}
	
	const FItemSlotData& SlotData = ItemSlot->GetItemSlotData();
	if(SlotData.ItemType == EItemType::Gear || SlotData.ItemType == EItemType::Weapon)
	{
		if(FItemData* ItemData = UUtil::GetItemDataFromName(SlotData.ItemName, FString("UGearSlot::CheckCanSwapWith")))
		{
			if(ItemData->GearData)
			{
				return ItemData->GearData->GearType == GearType;
			}
		}
	}
	return false;
}

void UGearSlot::NativeConstruct()
{
	Super::NativeConstruct();
	FString GearTypeString = UEnum::GetValueAsString(GearType);
	GearTypeString.RemoveFromStart("EGearType::");
	GearTypeText->SetText(FText::FromString(GearTypeString));
}
