// Copyright Radiant Studios


#include "Util/Interfaces/Carrier.h"

// Add default functionality here for any ICarrier functions that are not pure virtual.
void ICarrier::PickUpItem(const FString& ItemName)
{
	GetInventory()->AddItem(ItemName);
}

void ICarrier::DropItem(const FString& ItemName)
{
	GetInventory()->RemoveItem(ItemName);
}
