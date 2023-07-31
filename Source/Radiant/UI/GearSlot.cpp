#include "GearSlot.h"
#include "Components/Image.h"
#include "Data/GearData.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UGearSlot::SetEmpty(const bool Empty)
{
	Super::SetEmpty(Empty);

	if(Empty)
	{
		Icon->SetBrushFromTexture(EmptyGearIcon);
	}
}

bool UGearSlot::CheckCanSwapWith(UItemSlot* ItemSlot)
{
	if(ItemSlot == nullptr)
		return false;

	if(ItemSlot->IsEmpty())
	{
		GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
		return true;
	}
	
	const FItemSlotData& SlotData = ItemSlot->GetItemSlotData();
	if(SlotData.bIsGear)
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

void UGearSlot::SetData(const FItemSlotData& Data)
{
	Super::SetData(Data);

	if(Data.bIsGear)
	{
		GetOwningPlayerState<ARTPlayerState>()->S_EquipGear(ItemSlotData.ItemName);
	}
}
