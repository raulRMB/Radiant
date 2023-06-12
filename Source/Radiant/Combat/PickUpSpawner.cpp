// Copyright Radiant Studios


#include "Combat/PickUpSpawner.h"

#include "PickUp.h"

APickUpSpawner::APickUpSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APickUpSpawner::OnPickedUp()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &APickUpSpawner::SpawnPickup, SpawnTimer, false);
}

void APickUpSpawner::SpawnPickup()
{
	APickUp* Spawned = GetWorld()->SpawnActorDeferred<APickUp>(PickUpClass, GetActorTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Spawned->OnPickedUp.BindUObject(this, &APickUpSpawner::OnPickedUp);
	Spawned->FinishSpawning(GetActorTransform());
}

void APickUpSpawner::BeginPlay()
{
	Super::BeginPlay();
	if(InitialDelay <= 0)
	{
		SpawnPickup();
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &APickUpSpawner::SpawnPickup, InitialDelay, false);
	}
}

void APickUpSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

