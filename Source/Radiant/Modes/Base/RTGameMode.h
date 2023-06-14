// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/RadiantPlayerController.h"
#include "Util/Enums/TeamId.h"
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

	TSubclassOf<class AAvatar> HeroClass;	
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
	void HandleMatchHasEnded() override;
	void EndGame();
	void PlayersAreLoaded() const;
	void NotifyMatchEnd(ETeamId WinningTeam);
	void DisableAllAbilities();

	UFUNCTION(Exec)
	void StartGame();

	virtual bool ReadyToStartMatch_Implementation() override;
private:
	bool ReadyToEndMatch_Implementation();
	mutable bool bInitialPlayerLoad = false;

	virtual void HandleMatchHasStarted() override;

	class ARTPlayerStart* FindTeamStartTransform(ETeamId TeamId);
public:
	void Respawn(class ARadiantPlayerController* PlayerController);
};
