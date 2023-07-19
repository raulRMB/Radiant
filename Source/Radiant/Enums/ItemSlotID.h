﻿#pragma once

#include "CoreMinimal.h"
#include "ItemSlotID.generated.h"

UENUM(BlueprintType)
enum class EItemSlotID : uint8
{
	HotBarFirst = 0,
	HotBarTwo,
	HotBarThree,
	HotBarFour,
	HotBarFive,
	HotBarLast,
	InventoryFirst,
	InventoryTwo,
	InventoryThree,
	InventoryFour,
	InventoryFive,
	InventorySix,
	InventorySeven,
	InventoryEight,
	InventoryNine,
	InventoryTen,
	InventoryEleven,
	InventoryTwelve,
	InventoryThirteen,
	InventoryFourteen,
	InventoryFifteen,
	InventorySixteen,
	InventorySeventeen,
	InventoryEighteen,
	InventoryNineteen,
	InventoryLast,
	WeaponSlot,
	SlotAmount,
};
