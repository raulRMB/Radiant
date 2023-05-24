// Copyright Radiant Studios


#include "Combat/TargetedProjectile.h"

#include "GameFramework/CharacterMovementComponent.h"

void ATargetedProjectile::MulticastSetTarget_Implementation(AActor* NewTarget)
{
	Target = NewTarget;
}

// Sets default values
ATargetedProjectile::ATargetedProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ATargetedProjectile::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
		MulticastSetTarget(Target);
}

// Called every frame
void ATargetedProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Target)
	{
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		SetActorLocation(GetActorLocation() + (Direction.GetSafeNormal() * Speed * DeltaTime));
	}
}

