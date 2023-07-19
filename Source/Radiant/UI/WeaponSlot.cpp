// Copyright Radiant Studios


#include "UI/WeaponSlot.h"

#include "Components/Image.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void UWeaponSlot::SetEmpty(const bool Empty)
{
	Super::SetEmpty(Empty);

	if(Empty)
	{
		Icon->SetBrushFromTexture(EmptyHandsIcon);
	}
}

bool UWeaponSlot::CheckCanSwapWith(UItemSlot* ItemSlot)
{
	if(ItemSlot == nullptr)
		return false;

	if(ItemSlot->IsEmpty())
		return true;
	
	const FItemSlotData& SlotData = ItemSlot->GetItemSlotData();
	return SlotData.bIsWeapon;
}

void UWeaponSlot::SetData(const FItemSlotData& Data)
{
	Super::SetData(Data);

	if(Data.bIsWeapon)
	{
		if(FItemData* ItemData = UUtil::GetItemDataFromName(ItemSlotData.ItemName))
		{
			GetOwningPlayerState<ARTPlayerState>()->S_EquipWeapon(ItemData->AbilityData);
		}
	}
}
