// Copyright Radiant Studios


#include "Util/Interfaces/Carrier.h"
#include "Player/InventoryComponent.h"
#include "Util/Util.h"

// Add default functionality here for any ICarrier functions that are not pure virtual.
void ICarrier::PickUpItem(const FName& ItemName, const uint32 Amount)
{
	FItemData* ItemData = UUtil::GetItemDataFromName(ItemName);
	GetInventory()->AddItem(ItemName, ItemData, Amount);
}

void ICarrier::DropItem(const FName& ItemName)
{
	GetInventory()->S_DropItem(ItemName);
}
