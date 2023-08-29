// Copyright Radiant Studios


#include "RTGameState.h"

#include "RTGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Net/UnrealNetwork.h"
#include "Objectives/CaptureArea.h"
#include "Player/RTPlayerState.h"
#include "UI/CaptureAreaBar.h"
#include "Util/Util.h"
#include "Util/Managers/GridManager.h"

void ARTGameState::OnHeroDeath_Implementation(AAvatar* Hero)
{
	ETeamId TeamID = Hero->GetPlayerState<ARTPlayerState>()->GetTeamId();
	TeamID == ETeamId::Blue ? RedScore++ : BlueScore++;
	UE_LOG(LogTemp, Warning, TEXT("Red: %d, Blue: %d"), RedScore, BlueScore);
	NotifyHeroDeath(RedScore, BlueScore);
}

void ARTGameState::NotifyHeroDeath_Implementation(uint32 Score1, uint32 Score2)
{
	OnHeroDeathDelegate.ExecuteIfBound(Score1, Score2);
}

ARTGameState::ARTGameState()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARTGameState::DepositRadianite_Implementation(ETeamId TeamID, uint32 Radianite)
{
	if(TeamID == ETeamId::Blue)
	{
		BlueRadianite += Radianite;
		while(BlueUntilNextLevel <= BlueRadianite)
		{
			PreviousBlueNextLevel = BlueUntilNextLevel;
			BlueUntilNextLevel *= RadianiteToLevelMultiplier;
			BlueLevel++;
		}
		double AmountNeeded = BlueUntilNextLevel - PreviousBlueNextLevel;
		double AmountGained = static_cast<double>(BlueRadianite) - PreviousBlueNextLevel;
		NotifyRadianiteChanged(TeamID, AmountGained / AmountNeeded, BlueLevel);
	}
	else
	{
		RedRadianite += Radianite;
		while(RedUntilNextLevel <= RedRadianite)
		{
			PreviousRedNextLevel = RedUntilNextLevel;
			RedUntilNextLevel *= RadianiteToLevelMultiplier;
			RedLevel++;
		}
		double AmountNeeded = RedUntilNextLevel - PreviousRedNextLevel;
		double AmountGained = static_cast<double>(RedRadianite) - PreviousRedNextLevel;
		NotifyRadianiteChanged(TeamID, AmountGained / AmountNeeded, RedLevel);
	}
}

void ARTGameState::NotifyRadianiteChanged_Implementation(ETeamId TeamId, double XP, uint32 Level)
{	
	if(TeamId == ETeamId::Blue)
	{
		OnBlueRadianiteChangedDelegate.Broadcast(XP, Level);
	}
	else
	{
		OnRedRadianiteChangedDelegate.Broadcast(XP, Level);
	}
}

void ARTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARTGameState, BlueScore, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, RedScore, COND_None);
	
	DOREPLIFETIME_CONDITION(ARTGameState, BlueLevel, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, RedLevel, COND_None);

	DOREPLIFETIME_CONDITION(ARTGameState, BlueRadianite, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, RedRadianite, COND_None);
}

void ARTGameState::BeginPlay()
{
	Super::BeginPlay();
}
