// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TempGridActor.generated.h"

UCLASS()
class RADIANT_API ATempGridActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Box;
	
public:
	ATempGridActor();

	void SetMesh(const UStaticMeshComponent* NewMesh) const;
	void SetMaterial(UMaterialInterface* NewMaterial) const;
	void SetBox(const UBoxComponent* NewBox, const FVector& Location) const;
};
