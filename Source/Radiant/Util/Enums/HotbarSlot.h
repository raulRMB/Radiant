#pragma once

#include "CoreMinimal.h"
#include "HotBarSlot.generated.h"

UENUM(BlueprintType)
enum class EHotBarSlot : uint8
{
	One = 0,
	Two = 1,
	Three = 2,
	Four = 3,
	Five = 4,
	Six = 5,
	InventoryOne = 90,
	InventoryTwo = 91,
	InventoryThree = 92,
	InventoryFour = 93,
	InventoryFive = 94,
	InventorySix = 95,
	InventorySeven = 96,
	InventoryEight = 97,
	InventoryNine = 98,
	InventoryTen = 99,
	InventoryEleven = 100,
	InventoryTwelve = 101,
	InventoryThirteen = 102,
	InventoryFourteen = 103,
	InventoryFifteen = 104,
	InventorySixteen = 105,
	InventorySeventeen = 106,
	InventoryEighteen = 107,
	InventoryNineteen = 108,
	InventoryTwenty = 109,
	None = 255,
};
