// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = EArtilleryBehavior))
	int32 Behavior = 0;

	bool ShouldHit(AActor* OtherActor);
};
