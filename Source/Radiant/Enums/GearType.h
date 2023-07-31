#pragma once

#include "ItemSlotID.h"

UENUM()
enum class EGearType : uint8
{
	Weapon		= EItemSlotID::WeaponSlot,
	Helmet		= EItemSlotID::HelmetSlot,
	Chest		= EItemSlotID::ChestSlot,
	Legs		= EItemSlotID::LegsSlot,
	Boots		= EItemSlotID::BootsSlot,
	Gloves		= EItemSlotID::GlovesSlot,
	Necklace	= EItemSlotID::NecklaceSlot
};