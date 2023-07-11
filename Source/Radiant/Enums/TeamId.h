// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "TeamId.generated.h"

UENUM(BlueprintType)
enum class ETeamId : uint8
{
	Red			= 0,
	Blue		= 1,
	Neutral 	= 2
};