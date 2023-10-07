#pragma once

#include "CoreMinimal.h"
#include "ClassType.generated.h"

UENUM(BlueprintType)
enum class EClassType : uint8
{
	General,
	Support,
	Damage,
	Tank
};
