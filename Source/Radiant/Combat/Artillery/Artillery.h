// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "Artillery.generated.h"

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EArtilleryBehavior : uint8
{
	None			= 0 UMETA(Hidden),
	HitSelf			= 1 << 0,
	HitAllies		= 1 << 1,
	HitEnemies		= 1 << 2,
	HitTerrain		= 1 << 3,
	HitBuildings	= 1 << 4
};
ENUM_CLASS_FLAGS(EArtilleryBehavior)

UCLASS()
class AArtillery : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = EArtilleryBehavior))
	int32 Behavior = 1;
	
	bool ShouldHit(AActor* OtherActor);
};
