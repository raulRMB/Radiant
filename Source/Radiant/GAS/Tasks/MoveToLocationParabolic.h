// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "MoveToLocationParabolic.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API UMoveToLocationParabolic : public UAbilityTask_MoveToLocation
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	TObjectPtr<UCurveVector> ParabolicCurve;
	UPROPERTY(Replicated)
	float Height;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void TickTask(float DeltaTime) override;
public:
	static UMoveToLocationParabolic* MoveToLocationParabolic(UGameplayAbility* OwningAbility, FName TaskInstanceName, FVector Location, float Duration, UCurveFloat* OptionalInterpolationCurve, UCurveVector* OptionalVectorInterpolationCurve, UCurveVector* ParabolicCurve, float Height);
	
};
