#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums/GearType.h"
#include "GearData.generated.h"

UCLASS()
class RADIANT_API UGearData : public UDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UGameplayEffect>> GameplayEffects;
	
	UPROPERTY(EditAnywhere)
	EGearType GearType;
};
