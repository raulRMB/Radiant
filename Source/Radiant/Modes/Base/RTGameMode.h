// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/RTPlayerController.h"
#include "Enums/TeamId.h"
#include "RTGameMode.generated.h"


UCLASS()
class RADIANT_API ARTGameMode : public AGameMode
{
	GENERATED_BODY()

	uint8 bMatchIsOver : 1;

public:
	ARTGameMode();
	void SetTeamSize(FString QueueName);
	uint32 TeamSize = UINT_MAX;
	uint32 TeamCount = 2;

	uint32 PlayersLoaded = 0;

	void PlayerLoaded();

	void SetMatchOver(ETeamId WinningTeam);

	void SpawnAvatar(class ARTPlayerController* PlayerController);

	UPROPERTY(EditAnywhere, Category="Gameplay")
	TSubclassOf<class AAvatar> HeroClass;

protected:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void HandleMatchHasEnded() override;
	void EndGame();
	void PlayersAreLoaded() const;
	void NotifyMatchEnd(ETeamId WinningTeam);
	void DisableAllAbilities();

	UFUNCTION(Exec)
	void StartGame();

	virtual bool ReadyToStartMatch_Implementation() override;

private:
	virtual bool ReadyToEndMatch_Implementation() override;
	mutable bool bInitialPlayerLoad = false;
	bool TeamSizeSet = false;
	virtual void HandleMatchHasStarted() override;
	ETeamId WinningTeam = ETeamId::Neutral;

	class ARTPlayerStart* FindTeamStartTransform(ETeamId TeamId);

public:
	void Respawn(class ARTPlayerController* PlayerController);
};
