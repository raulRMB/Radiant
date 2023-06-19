// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	bool IsAuto;

	UPROPERTY(EditAnywhere)
	FText Tooltip;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayAbility> Ability;
	
};
