// Copyright Radiant Studios


#include "RTPlayerStart.h"
#include "Net/UnrealNetwork.h"

void ARTPlayerStart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerStart, TeamId);
	DOREPLIFETIME(ARTPlayerStart, bOccupied);
}
