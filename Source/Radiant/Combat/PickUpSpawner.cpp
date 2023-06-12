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
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &APickUpSpawner::SpawnPickup, 5, false);
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
	SpawnPickup();
}

void APickUpSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

