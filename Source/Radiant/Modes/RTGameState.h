// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RTGameState.generated.h"

USTRUCT()
struct FTeam
{
	GENERATED_BODY()
	
	uint8 TeamID : 1;
	int Score;
	TArray<class AHero*> Heroes;
};

/**
 * 
 */
UCLASS()
class RADIANT_API ARTGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FTeam RedTeam;
	UPROPERTY(VisibleAnywhere)
	FTeam BlueTeam;
};
