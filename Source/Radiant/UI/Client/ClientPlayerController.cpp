// Copyright Radiant Studios


#include "UI/Client/ClientPlayerController.h"

void AClientPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}