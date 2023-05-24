// Copyright Epic Games, Inc. All Rights Reserved.


#include "RadiantGameModeBase.h"
#include "Character/Hero.h"
#include "Character/RadiantPlayerController.h"

void ARadiantGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if(ARadiantPlayerController* PC = Cast<ARadiantPlayerController>(NewPlayer))
	{
		PC->SetPlayerID(PlayerCount);
		PlayerControllers.Add(PC);
		PlayerCount++;
	}
}

AHero* ARadiantGameModeBase::GetHeroFromPlayerID(int PlayerID)
{
	return PlayerControllers[PlayerID]->GetPawn<AHero>();
}
