// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS/Abilities/RTAbility.h"
#include "AbilityDataAsset.generated.h"


UCLASS()
class RADIANT_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	bool IgnoreHud;

	UPROPERTY(EditAnywhere)
	FText Tooltip;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class URTAbility> Ability;

	UPROPERTY(EditAnywhere)
	float Price;
};
