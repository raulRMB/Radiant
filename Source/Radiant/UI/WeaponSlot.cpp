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
	{
		GetOwningPlayerState<ARTPlayerState>()->S_UnequipWeapon();
		return true;
	}
	
	const FItemSlotData& SlotData = ItemSlot->GetItemSlotData();
	return SlotData.bIsWeapon;
}

void UWeaponSlot::SetData(const FItemSlotData& Data)
{
	Super::SetData(Data);

	if(Data.bIsWeapon)
	{
		GetOwningPlayerState<ARTPlayerState>()->S_EquipWeapon(ItemSlotData.ItemName);
	}
}