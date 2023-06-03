// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RTAbility.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API URTAbility : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability", meta=(AllowPrivateAccess="true"))
	float Range;

	FVector MouseWorldLocation;
public:
	const FVector& GetMouseWorldLocation() const { return MouseWorldLocation; }
	FVector GetRangeBasedMouseLocation();
	void SetMouseWorldLocation(const FVector& Location) { MouseWorldLocation = Location; }
};
