// Copyright Radiant Studios


#include "Util/Interfaces/Carrier.h"
#include "Player/InventoryComponent.h"

// Add default functionality here for any ICarrier functions that are not pure virtual.
void ICarrier::PickUpItem(const FName& ItemName, const uint32 Amount)
{
	GetInventory()->AddItem(ItemName, Amount);
}

void ICarrier::DropItem(const FName& ItemName)
{
	GetInventory()->S_DropItem(ItemName);
}
