// Copyright Radiant Studios

#include "TempGridActor.h"
#include "Components/BoxComponent.h"

ATempGridActor::ATempGridActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionProfileName("NoCollision");
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
	
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
}

void ATempGridActor::SetMesh(const UStaticMeshComponent* NewMesh) const
{
	Mesh->SetStaticMesh(NewMesh->GetStaticMesh());
}

void ATempGridActor::SetMaterial(UMaterialInterface* NewMaterial) const
{
	Mesh->SetMaterial(0, NewMaterial);
}

void ATempGridActor::SetBox(const UBoxComponent* NewBox, const FVector& Location) const
{
	Box->SetBoxExtent(NewBox->GetScaledBoxExtent());
	Box->SetWorldLocation(NewBox->GetRelativeLocation() + Location);
	// Box->SetHiddenInGame(false);

	Box->SetCollisionEnabled(NewBox->GetCollisionEnabled());
	Box->SetCollisionObjectType(NewBox->GetCollisionObjectType());

	Box->SetCollisionResponseToChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, NewBox->GetCollisionResponseToChannel(ECC_Pawn));
	Box->SetCanEverAffectNavigation(NewBox->CanEverAffectNavigation());
}
