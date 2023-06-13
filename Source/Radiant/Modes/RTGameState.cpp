// Copyright Radiant Studios


#include "RTGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Net/UnrealNetwork.h"
#include "Objectives/CaptureArea.h"
#include "Player/RTPlayerState.h"
#include "UI/CaptureAreaBar.h"
#include "UI/RTHUD.h"

void ARTGameState::OnHeroDeath_Implementation(AAvatar* Hero)
{
	ETeamId TeamID = Hero->GetPlayerState<ARTPlayerState>()->GetTeamId();
	TeamID == ETeamId::Red ? RedScore++ : BlueScore++;
	UE_LOG(LogTemp, Warning, TEXT("Red: %d, Blue: %d"), RedScore, BlueScore);
	NotifyHeroDeath(RedScore, BlueScore);
}

void ARTGameState::NotifyHeroDeath_Implementation(uint32 Score1, uint32 Score2)
{
	OnHeroDeathDelegate.ExecuteIfBound(Score1, Score2);
}

ARTGameState::ARTGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARTGameState, RedScore, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, BlueScore, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, RedTeamInArea, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, BlueTeamInArea, COND_None);
	DOREPLIFETIME_CONDITION(ARTGameState, CaptureAreaTeam, COND_None);
}

void ARTGameState::BeginPlay()
{
	Super::BeginPlay();

	CaptureArea = Cast<ACaptureArea>(UGameplayStatics::GetActorOfClass(this, ACaptureArea::StaticClass()));
	if(!HasAuthority())
	{
		CaptureAreaBar = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<ARTHUD>()->CaptureAreaBar;
	}
	else
	{
		CaptureArea->OnUpdatePlayersInArea.AddUObject(this, &ARTGameState::OnCaptureAreaUpdate);
	}
}

void ARTGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCaptureAreaPercent(DeltaSeconds);
}

void ARTGameState::OnCaptureAreaUpdate(ETeamId TeamId, bool bEntered)
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

void ARTGameState::UpdateCaptureAreaPercent(float DeltaTime)
{
	if(RedTeamInArea == 0)
	{
		CaptureAreaPercent += 0.05f * DeltaTime * BlueTeamInArea;
	}
	else if(BlueTeamInArea == 0)
	{
		CaptureAreaPercent -= 0.05f * DeltaTime * RedTeamInArea;
	}
	
	if(CaptureAreaBar)
	{
		CaptureAreaBar->SetTeamPercent(CaptureAreaTeam, CaptureAreaPercent);
	}
}
