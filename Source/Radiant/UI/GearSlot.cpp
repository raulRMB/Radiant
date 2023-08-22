#include "GearSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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
	if(SlotData.ItemType == EItemType::Gear || SlotData.ItemType == EItemType::Weapon)
	{
		if(FItemData* ItemData = UUtil::GetItemDataFromName(SlotData.ItemName, FString("UGearSlot::CheckCanSwapWith")))
		{
			if(ItemData->GearData)
			{
				const bool CanSwap = ItemData->GearData->GearType == GearType;
				if(CanSwap)
				{
					GetOwningPlayerState<ARTPlayerState>()->S_UnequipGear(ItemSlotData.ItemName);
				}
				return CanSwap;
			}
		}
	}
	return false;
}

void UGearSlot::SetData(const FItemSlotData& Data)
{
	Super::SetData(Data);

	if(Data.ItemType == EItemType::Gear || Data.ItemType == EItemType::Weapon)
	{
		GetOwningPlayerState<ARTPlayerState>()->S_EquipGear(ItemSlotData.ItemName);
	}
}

void UGearSlot::NativeConstruct()
{
	Super::NativeConstruct();
	FString GearTypeString = UEnum::GetValueAsString(GearType);
	GearTypeString.RemoveFromStart("EGearType::");
	GearTypeText->SetText(FText::FromString(GearTypeString));
}
