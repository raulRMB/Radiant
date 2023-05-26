// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RTGameState.generated.h"

DECLARE_DELEGATE_TwoParams(FOnHeroDeath, uint32, uint32);

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

public:
	UPROPERTY(Replicated, VisibleInstanceOnly)
	uint32 RedScore;
	UPROPERTY(Replicated, VisibleInstanceOnly)
	uint32 BlueScore;
	
	UFUNCTION(Server, Reliable)
	void OnHeroDeath(AHero* Hero);

	UFUNCTION(NetMulticast, Reliable)
	void NotifyHeroDeath(uint32 Score1, uint32 Score2);
	
	FOnHeroDeath OnHeroDeathDelegate;
	
	virtual void GetLifetimeReplicatedProps (TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
