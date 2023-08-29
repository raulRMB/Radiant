// Copyright Radiant Studios


#include "UI/TeamInfoDisplay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameState.h"
#include "Util/Util.h"

void UTeamInfoDisplay::Init()
{	
	if(ARTGameState* GS = Cast<ARTGameState>(UGameplayStatics::GetGameState(this)))
	{
		GS->OnBlueRadianiteChangedDelegate.AddUObject(this, &UTeamInfoDisplay::OnBlueRadianiteChanged);
		GS->OnRedRadianiteChangedDelegate.AddUObject(this, &UTeamInfoDisplay::OnRedRadianiteChanged);
	}
}

void UTeamInfoDisplay::OnRedRadianiteChanged(double XP, uint32 Level)
{
	RedLevel->SetText(RTPRINTF("%d", Level));
	RedXPBar->SetPercent(XP);
}

void UTeamInfoDisplay::OnBlueRadianiteChanged(double XP, uint32 Level)
{
	BlueLevel->SetText(RTPRINTF("%d", Level));
	BlueXPBar->SetPercent(XP);
}
