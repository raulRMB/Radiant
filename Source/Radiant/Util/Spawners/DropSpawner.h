// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropSpawner.generated.h"

DECLARE_DELEGATE(FPickedupSignature)

UCLASS()
class RADIANT_API ADropSpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickUp> PickUpClass;

	UPROPERTY(EditAnywhere)
	float SpawnRange;

	UPROPERTY(EditAnywhere)
	float SpawnRate;

	FPickedupSignature OnPickedUp;

	int Items;
public:	
	ADropSpawner();

protected:
	void SpawnPickup();

	UFUNCTION()
	void ItemPickedUp();
	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
