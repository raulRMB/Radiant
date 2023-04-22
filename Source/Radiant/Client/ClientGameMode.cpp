// // Copyright Radiant Studios
//
//
#include "ClientGameMode.h"

#include "ClientSubsystem.h"
#include "../Util/WidgetManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AClientGameMode::AClientGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetGameInstance()->GetSubsystem<UClientSubsystem>()->Setup();
}