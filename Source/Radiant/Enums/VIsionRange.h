#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EVisionRange : uint8
{
	Short = 0,
	Medium,
	Long,
	Count
};