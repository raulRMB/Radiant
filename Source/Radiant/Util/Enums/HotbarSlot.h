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
	None = 255,
};
