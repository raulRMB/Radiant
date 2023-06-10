// Copyright Radiant Studios


#include "RTGameState.h"

#include "RTGameMode.h"
#include "Character/Avatar.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"

void ARTGameState::OnHeroDeath_Implementation(AAvatar* Hero)
{
	uint32 TeamID = Cast<ARTPlayerState>(Hero->GetPlayerState())->TeamId;
	TeamID == 0 ? RedScore++ : BlueScore++;
	UE_LOG(LogTemp, Warning, TEXT("Red: %d, Blue: %d"), RedScore, BlueScore);
	NotifyHeroDeath(RedScore, BlueScore);
}

void ARTGameState::NotifyHeroDeath_Implementation(uint32 Score1, uint32 Score2)
{
	OnHeroDeathDelegate.ExecuteIfBound(Score1, Score2);
}

void ARTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARTGameState, RedScore, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, BlueScore, COND_None);
}
