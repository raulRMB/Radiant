// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilityDataAsset.h"
#include "WeaponDataAsset.generated.h"

UCLASS()
class RADIANT_API UWeaponDataAsset : public UAbilityDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float AttackRange;
};