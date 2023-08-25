#pragma once

#include "ItemSlotID.h"

UENUM()
enum class EGearType : uint8
{
	Weapon		= static_cast<uint8>(EItemSlotID::WeaponSlot),
	Helmet		= static_cast<uint8>(EItemSlotID::HelmetSlot),
	Chest		= static_cast<uint8>(EItemSlotID::ChestSlot),
	Legs		= static_cast<uint8>(EItemSlotID::LegsSlot),
	Boots		= static_cast<uint8>(EItemSlotID::BootsSlot),
	Gloves		= static_cast<uint8>(EItemSlotID::GlovesSlot),
	Necklace	= static_cast<uint8>(EItemSlotID::NecklaceSlot)
};