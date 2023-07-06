// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Util/Managers/GridManager.h"
#include "BuildingTypes.generated.h"


UCLASS()
class RADIANT_API UBuildingTypes : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<EEnvironmentType, TSubclassOf<class AActor>> EnvironmentTypes;
};
