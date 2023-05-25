// Copyright Radiant Studios


#include "RadiantPlayerController.h"

#include "Hero.h"
#include "Net/UnrealNetwork.h"

void ARadiantPlayerController::OnRep_PlayerID()
{
	if(GetPawn())
	{
		Cast<AHero>(GetPawn())->S_SetPlayerID(PlayerID);
	}
}

void ARadiantPlayerController::OnRep_TeamID()
{
	if(GetPawn())
	{
		Cast<AHero>(GetPawn())->S_SetTeamID(TeamID);
	}
}

void ARadiantPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ARadiantPlayerController, PlayerID, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ARadiantPlayerController, TeamID, COND_None, REPNOTIFY_OnChanged);
}
