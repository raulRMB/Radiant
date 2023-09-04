﻿// Copyright Radiant Studios


#include "TempGridActor.h"


// Sets default values
ATempGridActor::ATempGridActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ATempGridActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATempGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

