// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TempGridActor.generated.h"

UCLASS()
class RADIANT_API ATempGridActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	UStaticMeshComponent* Mesh;
	
public:
	// Sets default values for this actor's properties
	ATempGridActor();
	const UStaticMeshComponent* GetMesh() const { return Mesh; }
	void SetMesh(UStaticMeshComponent *NewMesh)
	{
		Mesh->SetStaticMesh(NewMesh->GetStaticMesh());
		Mesh->SetWorldScale3D(NewMesh->GetComponentScale());
		Mesh->SetRelativeRotation(NewMesh->GetRelativeRotation());
		Mesh->SetRelativeLocation(NewMesh->GetRelativeLocation());
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
