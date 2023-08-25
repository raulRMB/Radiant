// Copyright Radiant Studios


#include "RTGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Net/UnrealNetwork.h"
#include "Objectives/CaptureArea.h"
#include "Player/RTPlayerState.h"
#include "UI/CaptureAreaBar.h"
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

void ARTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARTGameState, RedScore, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, BlueScore, COND_None);
}

void ARTGameState::BeginPlay()
{
	Super::BeginPlay();
}
