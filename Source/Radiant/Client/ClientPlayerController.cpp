// Copyright Radiant Studios


#include "Client/ClientPlayerController.h"

void AClientPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}