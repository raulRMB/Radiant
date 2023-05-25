// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RadiantGameModeBase.generated.h"

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
class RADIANT_API ARadiantGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	int PlayerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	TArray<class ARadiantPlayerController*> PlayerControllers;

	UPROPERTY(VisibleAnywhere)
	FTeam RedTeam;
	UPROPERTY(VisibleAnywhere)
	FTeam BlueTeam;
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;

private:
	friend class UUtil;
	class AHero* GetHeroFromPlayerID(int PlayerID);
};
