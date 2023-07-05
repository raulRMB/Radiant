// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTGameMode.h"

#include "RTGameState.h"
#include "RTPlayerStart.h"
#include "Player/Avatar.h"
#include "GameFramework/GameSession.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RTPlayerState.h"
#include "Util/Managers/ActorManager.h"
#include "Util/Spawners/AISpawner.h"

ARTGameMode::ARTGameMode()
{
}

void ARTGameMode::SetTeamSize(FString QueueName)
{
	if(!TeamSizeSet && TeamSize == UINT_MAX)
	{
		if(QueueName == "1v1")
		{
			TeamSize = 1;
			UE_LOG(LogTemp, Warning, TEXT("Set Team Size - 1v1"))
		}
		else if(QueueName == "2v2")
		{
			TeamSize = 2;
			UE_LOG(LogTemp, Warning, TEXT("Set Team Size - 2v2"))
		}
		else if(QueueName == "3v3")
		{
			TeamSize = 3;
			UE_LOG(LogTemp, Warning, TEXT("Set Team Size - 3v3"))
		}
		else if(QueueName == "4v4")
		{
			TeamSize = 4;
			UE_LOG(LogTemp, Warning, TEXT("Set Team Size - 4v4"))
		}
		else if(QueueName == "5v5")
		{
			TeamSize = 5;
			UE_LOG(LogTemp, Warning, TEXT("Set Team Size - 5v5"))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Team Size not set"))
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
		PC->Connected();
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

void ARTGameMode::SetMatchOver(ETeamId WinningTeamId)
{
	WinningTeam = WinningTeamId;
	bMatchIsOver = true;
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
			Hero->GameReadyUnicast();
			if(HasAuthority())
			{
				Hero->ApplyInitialEffects();
			}
		}
	}
}

void ARTGameMode::NotifyMatchEnd(ETeamId WinningTeamId)
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() != nullptr))
		{
			AAvatar* Hero = Cast<AAvatar>(PlayerController->GetPawn());
			Hero->GameEnding(Hero->GetPlayerState<ARTPlayerState>()->GetTeamId() == WinningTeamId);
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
	if(bMatchIsOver)
	{
		NotifyMatchEnd(WinningTeam);
	}
	return bMatchIsOver;
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

	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAISpawner::StaticClass(), Spawners);
	for(AActor* Spawner : Spawners)
	{
		AAISpawner* AISpawner = Cast<AAISpawner>(Spawner);
		AISpawner->StartSpawning();
	}
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
		if(AAvatar* Hero = Cast<AAvatar>(PlayerController->GetPawn()))
		{
			if(UActorManager* Manager = GetGameInstance()->GetSubsystem<UActorManager>())
			{
				Manager->RemovePlayer(Hero);
				PlayerController->GetPawn()->Destroy();
				
				Hero = GetWorld()->SpawnActorDeferred<AAvatar>(HeroClass, PlayerController->GetPlayerStart()->GetTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				Hero->SetPlayerState(PlayerController->PlayerState);
				Hero->FinishSpawning(PlayerController->GetPlayerStart()->GetTransform());
				if(Hero)
				{
					PlayerController->Possess(Hero);
					Manager->AddPlayer(Hero);
				}
			}
		}
	}
}
