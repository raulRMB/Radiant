// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Character/RadiantPlayerController.h"
#include "RTGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ARTGameMode();
	
	uint32 TeamSize = INT_MAX;
	uint32 TeamCount = 2;
	uint32 PlayersLoaded = 0;
	UPROPERTY(EditAnywhere)
	uint32 KillsToWin = 3;
	void PlayerLoaded();

	void SpawnAvatar(class ARadiantPlayerController* PlayerController);

	TSubclassOf<class AHero> HeroClass;	
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
	void HandleMatchHasEnded() override;
	void EndGame();
	void PlayersAreLoaded() const;
	void NotifyMatchEnd(int32 WinningTeam);
	void DisableAllAbilities();

	UFUNCTION(Exec)
	void StartGame();

private:
	virtual bool ReadyToStartMatch_Implementation() override;
	bool ReadyToEndMatch_Implementation();
	mutable bool bInitialPlayerLoad = false;

	virtual void HandleMatchHasStarted() override;

	class ARTPlayerStart* FindTeamStartTransform(uint8 TeamId);
public:
	void Respawn(class ARadiantPlayerController* PlayerController);
};
