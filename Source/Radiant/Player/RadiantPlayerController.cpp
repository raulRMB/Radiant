// Copyright Radiant Studios

#include "RadiantPlayerController.h"
#include "Modes/RTGameMode.h"
#include "Net/UnrealNetwork.h"

void ARadiantPlayerController::PlayerLoaded_Implementation()
{
	ARTGameMode* GM = GetWorld()->GetAuthGameMode<ARTGameMode>();
	GM->PlayerLoaded();
}

void ARadiantPlayerController::S_SetPlayerStart_Implementation(ARTPlayerStart* NewPlayerStart)
{
	PlayerStart = NewPlayerStart;	
}

void ARadiantPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadiantPlayerController, PlayerStart)
}
