// Copyright Radiant Studios


#include "Modes/TeamDeathMatchMode.h"
#include "Base/RTGameState.h"

bool ATeamDeathMatchMode::ReadyToStartMatch_Implementation()
{
	ARTGameState* State = Cast<ARTGameState>(GetWorld()->GetGameState());
	if(State->RedScore >= KillsToWin * TeamSize)
	{
		NotifyMatchEnd(ETeamId::Red);
		return true;
	}
	if(State->BlueScore >= KillsToWin * TeamSize)
	{
		NotifyMatchEnd(ETeamId::Blue);
		return true;
	}
	return false;
}
