// Copyright Radiant Studios


#include "Building/MobSpawner.h"

#include "TimerManager.h"
#include "Characters/AI/RTAICharacter.h"
#include "Engine/World.h"

AMobSpawner::AMobSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMobSpawner::Spawn()
{
	if(Count >= MaxSpawned)
		return;
	Count++;
	float XOffset = FMath::RandRange(-1.0f, 1.0f);
	float YOffset = FMath::RandRange(-1.0f, 1.0f);
	FVector SpawnLocation = GetActorLocation() + (FVector(XOffset, YOffset, 0.0f) * SpawnRange);
	
	ARTAICharacter* Character = GetWorld()->SpawnActorDeferred<ARTAICharacter>(MobClass, FTransform(SpawnLocation));
	Character->OnUnitDied.AddUObject(this, &AMobSpawner::OnDeath);
	Character->FinishSpawning(FTransform(SpawnLocation));
}

void AMobSpawner::OnDeath()
{
	Count--;
}

void AMobSpawner::StartSpawning()
{
	if(HasAuthority())
	{
		Spawn();	
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &AMobSpawner::Spawn, SpawnRate, true);
	}
}

void AMobSpawner::BeginPlay()
{
	Super::BeginPlay();
	StartSpawning();
}

void AMobSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(Handle);
}