// Copyright Radiant Studios


#include "MeleeSpin.h"

#include "Components/StaticMeshComponent.h"


// Sets default values
AMeleeSpin::AMeleeSpin()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OuterTornadoMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("OuterTornadoMesh"));
	OuterTornadoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OuterTornadoMesh->SetupAttachment(RootComponent);
	OuterTornadoMesh->SetUsingAbsoluteRotation(true);	

	InnerTornadoMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("InnerTornadoMesh"));
	InnerTornadoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InnerTornadoMesh->SetupAttachment(RootComponent);
	InnerTornadoMesh->SetUsingAbsoluteRotation(true);
}

// Called when the game starts or when spawned
void AMeleeSpin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeleeSpin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(OuterTornadoMesh)
	{
		OuterTornadoMesh->AddLocalRotation(FRotator(0, OuterSpinSpeed * DeltaTime, 0));
	}

	if(InnerTornadoMesh)
	{
		InnerTornadoMesh->AddLocalRotation(FRotator(0, InnerSpinSpeed * DeltaTime, 0));
	}
}

