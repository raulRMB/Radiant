#pragma once

#include "CoreMinimal.h"
#include "EnvironmentType.generated.h"

UENUM(BlueprintType)
enum class EEnvironmentType : uint8
{	
	EEnvironmentType_Empty = 0,
	EEnvironmentType_Wall,
	EEnvironmentType_Tower,
	EEnvironmentType_Tree,
	EEnvironmentType_Rock,
	EEnvironmentType_TreeStump,
	EEnvironmentType_Pebbles,
	EEnvironmentType_Grass,
	EEvironmentType_MobSpawner,
};