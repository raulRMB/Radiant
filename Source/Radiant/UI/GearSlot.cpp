#include "GearSlot.h"

#include "TimerManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/GearData.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UGearSlot::EquipGear()
{
	if(auto PS = GetOwningPlayerState<ARTPlayerState>())
	{
		PS->S_EquipGear(ItemSlotData.ItemName);
	} else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGearSlot::EquipGear, 0.1f, false);
	}
}

void UGearSlot::OnAfterFill()
{
	Super::OnAfterFill();
	if(ValidateGear())
	{
		EquipGear();
	}
}

void UGearSlot::OnBeforeEmpty()
{
	Super::OnBeforeEmpty();
	if(ValidateGear())
	{
		GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
	}
}

void UGearSlot::OnBeforeSwap()
{
	Super::OnBeforeSwap();
	if(ValidateGear())
	{
		GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
	}
}

void UGearSlot::OnBeforeItemDropped()
{
	Super::OnBeforeItemDropped();
	if(ValidateGear())
	{
		GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
	}
}

bool UGearSlot::ValidateGear()
{
	if(ItemSlotData.ItemName.IsNone())
	{
		return false;
	}
	return true;
}

void UGearSlot::OnAfterSwap()
{
	Super::OnAfterSwap();
	if(ValidateGear())
	{
		GetOwningPlayerState<ARTPlayerState>()->S_EquipGear(ItemSlotData.ItemName);
	}
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
