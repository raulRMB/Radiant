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

	FVector MouseWorldLocation;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, Category="Ability")
	float MaxRange;
public:
	const FVector& GetMouseWorldLocation() const { return MouseWorldLocation; }
	FVector GetRangeBasedMouseLocation();
	void SetMouseWorldLocation(const FVector& Location) { MouseWorldLocation = Location; }
	FVector GetRangedBaseMouseLocationWithHeroHalfHeight();
	FVector GetAvatarToMouseDirection();
};
