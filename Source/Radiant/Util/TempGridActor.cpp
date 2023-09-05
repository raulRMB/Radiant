// Copyright Radiant Studios


#include "TempGridActor.h"


// Sets default values
ATempGridActor::ATempGridActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionProfileName("NoCollision");
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
	SetActorEnableCollision(false);
	RootComponent = Mesh;
}

void ATempGridActor::SetMesh(const UStaticMeshComponent* NewMesh) const
{
	Mesh->SetStaticMesh(NewMesh->GetStaticMesh());
}

void ATempGridActor::SetMaterial(UMaterialInterface* NewMaterial) const
{
	Mesh->SetMaterial(0, NewMaterial);
}

// Called when the game starts or when spawned
void ATempGridActor::BeginPlay()
{
	IsComponentRelevantForNavigation(nullptr);
	Super::BeginPlay();
}

// Called every frame
void ATempGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

