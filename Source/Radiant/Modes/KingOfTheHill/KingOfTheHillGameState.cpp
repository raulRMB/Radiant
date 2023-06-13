// Copyright Radiant Studios


#include "KingOfTheHillGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Objectives/CaptureArea.h"
#include "UI/CaptureAreaBar.h"
#include "UI/RTHUD.h"

void AKingOfTheHillGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AKingOfTheHillGameState, RedTeamInArea, COND_None);
	DOREPLIFETIME_CONDITION(AKingOfTheHillGameState, BlueTeamInArea, COND_None);
	DOREPLIFETIME_CONDITION(AKingOfTheHillGameState, CaptureAreaTeam, COND_None);
}

void AKingOfTheHillGameState::BeginPlay()
{
	Super::BeginPlay();

	CaptureArea = Cast<ACaptureArea>(UGameplayStatics::GetActorOfClass(this, ACaptureArea::StaticClass()));
	if(!HasAuthority())
	{
		CaptureAreaBar = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<ARTHUD>()->CaptureAreaBar;
	}
	else
	{
		if(CaptureArea)
			CaptureArea->OnUpdatePlayersInArea.AddUObject(this, &AKingOfTheHillGameState::OnCaptureAreaUpdate);
	}
}

void AKingOfTheHillGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCaptureAreaPercent(DeltaSeconds);
}

void AKingOfTheHillGameState::OnCaptureAreaUpdate(ETeamId TeamId, bool bEntered)
{
	switch (TeamId)
	{
	case ETeamId::Blue:
		bEntered ? BlueTeamInArea++ : BlueTeamInArea--;
		break;
	case ETeamId::Red:
		bEntered ? RedTeamInArea++ : RedTeamInArea--;
		break;
	default:
		break;
	}
	
	if(RedTeamInArea == 0 && BlueTeamInArea > 0)
	{
		CaptureAreaTeam = ETeamId::Blue;
	}
	else if(BlueTeamInArea == 0 && RedTeamInArea > 0)
	{
		CaptureAreaTeam = ETeamId::Red;
	}
	else
	{
		CaptureAreaTeam = ETeamId::Neutral;
	}
}

void AKingOfTheHillGameState::UpdateCaptureAreaPercent(float DeltaSeconds)
{
	if(RedTeamInArea == 0)
	{
		CaptureAreaPercent += 0.05f * DeltaSeconds * BlueTeamInArea;
	}
	else if(BlueTeamInArea == 0)
	{
		CaptureAreaPercent -= 0.05f * DeltaSeconds * RedTeamInArea;
	}
	
	if(CaptureAreaBar)
	{
		CaptureAreaBar->SetTeamPercent(CaptureAreaTeam, CaptureAreaPercent);
	}
}
