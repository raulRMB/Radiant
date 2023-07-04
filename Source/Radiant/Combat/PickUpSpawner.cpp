// Copyright Radiant Studios


#include "Combat/PickUpSpawner.h"

#include "PickUp.h"
#include "Util/Util.h"

APickUpSpawner::APickUpSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APickUpSpawner::OnPickedUp()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &APickUpSpawner::SpawnPickup, SpawnTimer, false);
}

void APickUpSpawner::SpawnPickup()
{
	if(HasAuthority())
	{
		APickUp* Spawned = GetWorld()->SpawnActorDeferred<APickUp>(PickUpClass, FTransform(GetActorLocation() + SpawnOffset), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		RTLOGP("Spawn offset %s", *SpawnOffset.ToString());
		Spawned->OnPickedUp.BindUObject(this, &APickUpSpawner::OnPickedUp);
		Spawned->FinishSpawning(FTransform(GetActorLocation() + SpawnOffset));
	}
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
