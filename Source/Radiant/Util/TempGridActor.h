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
	void SetMesh(const UStaticMeshComponent* NewMesh) const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
