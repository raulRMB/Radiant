// Copyright Radiant Studios

#include "RadiantPlayerController.h"
#include "Modes/RTGameMode.h"

void ARadiantPlayerController::PlayerLoaded_Implementation()
{
	ARTGameMode* GM = GetWorld()->GetAuthGameMode<ARTGameMode>();
	GM->PlayerLoaded();
}