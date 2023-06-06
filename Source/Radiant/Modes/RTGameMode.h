// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RTGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint32 TeamSize = 1;
	uint32 TeamCount = 2;
	uint32 PlayersLoaded = 0;
	uint32 KillsToWin = 1;
	void PlayerLoaded();
	
protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
	void HandleMatchHasEnded() override;
	void EndGame();
	void PlayersAreLoaded() const;
	void NotifyMatchEnd(int32 WinningTeam);

	UFUNCTION(Exec)
	void StartGame();

private:
	virtual bool ReadyToStartMatch_Implementation() override;
	bool ReadyToEndMatch_Implementation();

	virtual void HandleMatchHasStarted() override;
};
