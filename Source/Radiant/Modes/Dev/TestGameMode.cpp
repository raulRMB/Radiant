// Copyright Radiant Studios


#include "TestGameMode.h"

void ATestGameMode::BeginPlay()
{
	Super::BeginPlay();

	TeamSize = TestTeamSize;
	TeamCount = TestTeamCount;
}
