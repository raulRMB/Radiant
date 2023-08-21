#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAOEApplicationType : uint8
{
    None = 0 UMETA(Hidden),
    Instant,
    Constant,
    OnOverlap,
    OnTimerEnd_Instant,
    OnTimerEnd_Constant,
};