// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTGameMode.h"

#include "RTGameState.h"
#include "RTPlayerStart.h"
#include "Player/Avatar.h"
#include "GameFramework/GameSession.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RTPlayerState.h"

ARTGameMode::ARTGameMode()
{
}

void ARTGameMode::SetTeamSize(FString QueueName)
{
	if(!TeamSizeSet)
	{
		if(QueueName == "1v1")
		{
			TeamSize = 1;
		}
		else if(QueueName == "2v2")
		{
			TeamSize = 2;
		}
		else if(QueueName == "3v3")
		{
			TeamSize = 3;
		}
		else if(QueueName == "4v4")
		{
			TeamSize = 4;
		}
		else if(QueueName == "5v5")
		{
			TeamSize = 5;
		}
		TeamSizeSet = true;
	}
}

void ARTGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if(ARTPlayerController* PC = Cast<ARTPlayerController>(NewPlayer))
	{
		PC->GetPlayerState<ARTPlayerState>()->SetTeamId(ETeamId(NumPlayers % TeamCount));
	}
}

void ARTGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	DisableAllAbilities();
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &ARTGameMode::EndGame, 5.f, false);
}

void ARTGameMode::EndGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ARTGameMode::PlayerLoaded()
{
	if(!bInitialPlayerLoad)
	{
		PlayersLoaded++;
		uint32 players = (TeamSize * TeamCount);
		if(PlayersLoaded >= players * players)
		{
			bInitialPlayerLoad = true;
			FTimerHandle Handle;
			GetWorldTimerManager().SetTimer(Handle, this, &ARTGameMode::PlayersAreLoaded, 1.f, false);
		}
	}
}

void ARTGameMode::SpawnAvatar(ARTPlayerController* PlayerController)
{
	if(!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("No Valid Player Controller For Spawn Avatar"))
		return;
	}
	if( PlayerController->GetPawn() != nullptr)
	{
		PlayerController->GetPawn()->Destroy();
	}

	ARTPlayerStart* PlayerStart = FindTeamStartTransform(PlayerController->GetPlayerState<ARTPlayerState>()->GetTeamId());
	AAvatar* Hero = GetWorld()->SpawnActor<AAvatar>(HeroClass, PlayerStart->GetActorTransform());
	PlayerController->Possess(Hero);
	PlayerController->S_SetPlayerStart(PlayerStart);
}

void ARTGameMode::PlayersAreLoaded() const
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() != nullptr))
		{
			AAvatar* Hero = Cast<AAvatar>(PlayerController->GetPawn());
			Hero->GameReady();
			Hero->ApplyInitialEffects();
			Hero->GameReadyUnicast();
		}
	}
}

void ARTGameMode::NotifyMatchEnd(ETeamId WinningTeam)
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() != nullptr))
		{
			AAvatar* Hero = Cast<AAvatar>(PlayerController->GetPawn());
			Hero->GameEnding(Hero->GetPlayerState<ARTPlayerState>()->GetTeamId() == WinningTeam);
		}
	}
}

void ARTGameMode::DisableAllAbilities()
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() != nullptr))
		{
			AAvatar* Hero = Cast<AAvatar>(PlayerController->GetPawn());
			Hero->GetAbilitySystemComponent()->CancelAllAbilities();
			Hero->GetAbilitySystemComponent()->ClearAllAbilities();
		}
	}
}

void ARTGameMode::StartGame()
{
	StartMatch();
}

bool ARTGameMode::ReadyToStartMatch_Implementation()
{
	return NumPlayers == TeamSize * TeamCount;
}

bool ARTGameMode::ReadyToEndMatch_Implementation()
{
	ARTGameState* State = Cast<ARTGameState>(GetWorld()->GetGameState());
	if(State->RedScore >= KillsToWin * TeamSize)
	{
		NotifyMatchEnd(ETeamId::Red);
		return true;
	}
	if(State->BlueScore >= KillsToWin * TeamSize)
	{
		NotifyMatchEnd(ETeamId::Blue);
		return true;
	}
	return false;
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
			ARTPlayerController* PC = Cast<ARTPlayerController>(PlayerController);
			SpawnAvatar(PC);
		}
	}

	// Make sure level streaming is up to date before triggering NotifyMatchStarted
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());

	// First fire BeginPlay, if we haven't already in waiting to start match
	GetWorldSettings()->NotifyBeginPlay();

	// Then fire off match started
	GetWorldSettings()->NotifyMatchStarted();
}

ARTPlayerStart* ARTGameMode::FindTeamStartTransform(ETeamId TeamId)
{
	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTPlayerStart::StaticClass(), Starts);
	for(AActor* Start : Starts)
	{
		ARTPlayerStart* PlayerStart = Cast<ARTPlayerStart>(Start);
		if(PlayerStart->TeamId == TeamId)
		{
			if(PlayerStart->bOccupied)
			{
				continue;
			}
			PlayerStart->bOccupied = true;
			return PlayerStart;
		}
	}
	return nullptr;
}

void ARTGameMode::Respawn(ARTPlayerController* PlayerController)
{
	if(PlayerController->GetPawn() != nullptr)
	{
		PlayerController->GetPawn()->Destroy();
	}
	AAvatar* Hero = GetWorld()->SpawnActor<AAvatar>(HeroClass, PlayerController->GetPlayerStart()->GetTransform());
	PlayerController->Possess(Hero);
}
