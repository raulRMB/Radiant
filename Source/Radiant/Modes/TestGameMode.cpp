// Copyright Radiant Studios


#include "Modes/TestGameMode.h"

void ATestGameMode::BeginPlay()
{
	Super::BeginPlay();

	TeamSize = TestTeamSize;
	TeamCount = TestTeamCount;
}
