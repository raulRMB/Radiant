// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Combat/Artillery/LinearSkillshot.h"
#include "Jook.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AJook : public ALinearSkillshot
{
	GENERATED_BODY()

	UPROPERTY()
	class UGAJook* JookAbility;
public:
	void SetAbility(UGAJook* NewJookAbility) { JookAbility = NewJookAbility; }
	
	virtual void OverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
