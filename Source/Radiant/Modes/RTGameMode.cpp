// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTGameMode.h"
#include "Character/Hero.h"
#include "Character/RadiantPlayerController.h"

void ARTGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if(ARadiantPlayerController* PC = Cast<ARadiantPlayerController>(NewPlayer))
	{
		PC->SetPlayerID(NumPlayers);
		PlayerControllers.Add(PC);
	}
}

AHero* ARTGameMode::GetHeroFromPlayerID(int PlayerID)
{
	return PlayerControllers[PlayerID]->GetPawn<AHero>();
}

void ARTGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

}
