// Copyright Radiant Studios


#include "Util/Spawners/DropSpawner.h"

#include "TimerManager.h"
#include "Combat/PickUp.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Util.h"

// Sets default values
ADropSpawner::ADropSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ADropSpawner::SpawnPickup()
{
	if(Items >= 500)
		return;
	Items++;
	float XOffset = FMath::RandRange(-1.0f, 1.0f);
	float YOffset = FMath::RandRange(-1.0f, 1.0f);
	FVector SpawnLocation = GetActorLocation() + (FVector(XOffset, YOffset, 0.0f) * SpawnRange);
	
	APickUp* Pickup = GetWorld()->SpawnActorDeferred<APickUp>(PickUpClass, FTransform(SpawnLocation));
	Pickup->OnPickedUp.BindUObject(this, &ADropSpawner::ItemPickedUp);
	Pickup->FinishSpawning(FTransform(SpawnLocation));
}

void ADropSpawner::ItemPickedUp()
{
	Items--;
}


void ADropSpawner::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ADropSpawner::SpawnPickup, SpawnRate, true);
	}
}

// Called every frame
void ADropSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

