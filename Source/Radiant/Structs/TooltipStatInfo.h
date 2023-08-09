#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "TooltipStatInfo.generated.h"

USTRUCT()
struct FTooltipStatInfo
{
    GENERATED_BODY()
    
    FName CellType; 
    float Value;
    TEnumAsByte<EGameplayModOp::Type> ModOp;

    FTooltipStatInfo()
    {
        CellType = FName();
        Value = 0;
        ModOp = EGameplayModOp::Additive;
    }
};