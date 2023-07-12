// Copyright Radiant Studios


#include "KingOfTheHillMode.h"
#include "KingOfTheHillGameState.h"

bool AKingOfTheHillMode::ReadyToEndMatch_Implementation()
{
	AKingOfTheHillGameState* State = GetWorld()->GetGameState<AKingOfTheHillGameState>();
	if (State->GetCaptureAreaPercent() <= 0.0f)
	{
		NotifyMatchEnd(ETeamId::Red);
		return true;
	}
	if (State->GetCaptureAreaPercent() >= 1.0f)
	{
		NotifyMatchEnd(ETeamId::Blue);
		return true;
	}
	return false;
}

void AKingOfTheHillMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	AKingOfTheHillGameState* State = GetWorld()->GetGameState<AKingOfTheHillGameState>();
	State->MatchEnded();
}
