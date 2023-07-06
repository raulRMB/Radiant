// Copyright Radiant Studios


#include "Util/Interfaces/Carrier.h"
#include "Player/InventoryComponent.h"

// Add default functionality here for any ICarrier functions that are not pure virtual.
void ICarrier::PickUpItem(const FName& ItemName)
{
	GetInventory()->AddItem(ItemName);
}

void ICarrier::DropItem(const FName& ItemName)
{
	GetInventory()->RemoveItem(ItemName);
}
