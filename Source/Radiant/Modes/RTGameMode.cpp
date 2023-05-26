// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTGameMode.h"
#include "Character/Hero.h"
#include "Character/RadiantPlayerController.h"
#include "GameFramework/GameSession.h"
#include "Player/RTPlayerState.h"

void ARTGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if(ARadiantPlayerController* PC = Cast<ARadiantPlayerController>(NewPlayer))
	{
		PC->GetPlayerState<ARTPlayerState>()->TeamId = NumPlayers % 2;
	}
}

void ARTGameMode::PlayerLoaded()
{
	PlayersLoaded++;
	uint32 players = (TeamSize * TeamCount);
	if(PlayersLoaded >= players * players)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ARTGameMode::PlayersAreLoaded, 1.f, false);
	}
}

void ARTGameMode::PlayersAreLoaded() const
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() != nullptr))
		{
			AHero* Hero = Cast<AHero>(PlayerController->GetPawn());
			Hero->GameReady();
		}
	}
}

bool ARTGameMode::ReadyToStartMatch_Implementation()
{
	return NumPlayers == TeamSize * 2;
}

void ARTGameMode::HandleMatchHasStarted()
{
	// Super::HandleMatchHasStarted();

	GameSession->HandleMatchHasStarted();

	// start human players first
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() == nullptr) && PlayerCanRestart(PlayerController))
		{
			RestartPlayer(PlayerController);
		}
	}

	// Make sure level streaming is up to date before triggering NotifyMatchStarted
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());

	// First fire BeginPlay, if we haven't already in waiting to start match
	GetWorldSettings()->NotifyBeginPlay();

	// Then fire off match started
	GetWorldSettings()->NotifyMatchStarted();
}
