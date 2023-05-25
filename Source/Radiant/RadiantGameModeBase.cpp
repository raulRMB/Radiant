// Copyright Epic Games, Inc. All Rights Reserved.


#include "RadiantGameModeBase.h"
#include "Character/Hero.h"
#include "Character/RadiantPlayerController.h"

void ARadiantGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if(PlayerCount % 2)
	{
		RedTeam.Heroes.Add(Cast<AHero>(NewPlayer->GetPawn()));
	}
	else
	{
		BlueTeam.Heroes.Add(Cast<AHero>(NewPlayer->GetPawn()));
	}
	
	if(ARadiantPlayerController* PC = Cast<ARadiantPlayerController>(NewPlayer))
	{
		PC->SetPlayerID(PlayerCount);
		PC->SetTeamID(PlayerCount % 2);
		PlayerControllers.Add(PC);
		PlayerCount++;
	}
}

AHero* ARadiantGameModeBase::GetHeroFromPlayerID(int PlayerID)
{
	return PlayerControllers[PlayerID]->GetPawn<AHero>();
}
