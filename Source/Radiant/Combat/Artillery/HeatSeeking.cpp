// Copyright Radiant Studios


#include "HeatSeeking.h"
#include "GameFramework/CharacterMovementComponent.h"

void AHeatSeeking::MulticastSetTarget_Implementation(AActor* NewTarget)
{
	Target = NewTarget;
}

// Sets default values
AHeatSeeking::AHeatSeeking()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AHeatSeeking::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
		MulticastSetTarget(Target);
}

void AHeatSeeking::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetReplicateMovement(true);
}

// Called every frame
void AHeatSeeking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Target)
	{
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		SetActorLocation(GetActorLocation() + (Direction.GetSafeNormal() * Speed * DeltaTime));
	}
}

