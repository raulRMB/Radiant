// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GCAChangeSize.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AGCAChangeSize : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SizeMultiplier;

	UPROPERTY()
	FVector OriginalScale;
	
private:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
};
