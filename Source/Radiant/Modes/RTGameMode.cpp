// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTGameMode.h"

#include "RTGameState.h"
#include "RTPlayerStart.h"
#include "Character/Hero.h"
#include "GameFramework/GameSession.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RTPlayerState.h"

ARTGameMode::ARTGameMode()
{
	static ConstructorHelpers::FClassFinder<AHero> PlayerControllerBPClass(TEXT("/Game/Blueprints/BP_Hero"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		HeroClass = PlayerControllerBPClass.Class;
	}
}

void ARTGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if(ARadiantPlayerController* PC = Cast<ARadiantPlayerController>(NewPlayer))
	{
		PC->GetPlayerState<ARTPlayerState>()->TeamId = NumPlayers % TeamCount;
		SpawnAvatar(PC);
	}
}

void ARTGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	FTimerHandle Handle1;
	GetWorldTimerManager().SetTimer(Handle1, this, &ARTGameMode::DisableAllAbilities, 4.f, false);
	FTimerHandle Handle2;
	GetWorldTimerManager().SetTimer(Handle2, this, &ARTGameMode::EndGame, 5.f, false);
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

void ARTGameMode::SpawnAvatar(ARadiantPlayerController* PlayerController)
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

	ARTPlayerStart* PlayerStart = FindTeamStartTransform(PlayerController->GetPlayerState<ARTPlayerState>()->TeamId);
	AHero* Hero = GetWorld()->SpawnActor<AHero>(HeroClass, PlayerStart->GetActorTransform());
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
			AHero* Hero = Cast<AHero>(PlayerController->GetPawn());
			Hero->GameReady();
			Hero->ApplyInitialEffects();
			Hero->GameReadyUnicast();
		}
	}
}

void ARTGameMode::NotifyMatchEnd(int32 WinningTeam)
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() != nullptr))
		{
			AHero* Hero = Cast<AHero>(PlayerController->GetPawn());
			Hero->GameEnding(Hero->GetPlayerState<ARTPlayerState>()->TeamId == WinningTeam);
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
			AHero* Hero = Cast<AHero>(PlayerController->GetPawn());
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
	if(State->RedScore >= KillsToWin)
	{
		NotifyMatchEnd(1);
		return true;
	}
	if(State->BlueScore >= KillsToWin)
	{
		NotifyMatchEnd(0);
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

ARTPlayerStart* ARTGameMode::FindTeamStartTransform(uint8 TeamId)
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

void ARTGameMode::Respawn(ARadiantPlayerController* PlayerController)
{
	if(PlayerController->GetPawn() != nullptr)
	{
		PlayerController->GetPawn()->Destroy();
	}
	AHero* Hero = GetWorld()->SpawnActor<AHero>(HeroClass, PlayerController->GetPlayerStart()->GetTransform());
	PlayerController->Possess(Hero);
}
