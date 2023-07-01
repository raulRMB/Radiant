// Copyright Radiant Studios


#include "Util/Spawners/AISpawner.h"

#include "Characters/AI/RTAICharacter.h"

AAISpawner::AAISpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAISpawner::Spawn()
{
	if(Count >= MaxSpawned)
		return;
	Count++;
	float XOffset = FMath::RandRange(-1.0f, 1.0f);
	float YOffset = FMath::RandRange(-1.0f, 1.0f);
	FVector SpawnLocation = GetActorLocation() + (FVector(XOffset, YOffset, 0.0f) * SpawnRange);
	
	ARTAICharacter* Character = GetWorld()->SpawnActorDeferred<ARTAICharacter>(AI, FTransform(SpawnLocation));
	Character->OnUnitDied.AddUObject(this, &AAISpawner::OnDeath);
	Character->FinishSpawning(FTransform(SpawnLocation));
}

void AAISpawner::OnDeath()
{
	Count--;
}

void AAISpawner::StartSpawning()
{
	if(HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &AAISpawner::Spawn, SpawnRate, true);
	}
}

void AAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

