// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GCASlow.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AGCASlow : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	class UMaterialInterface* SlowMaterial;
private:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
};
