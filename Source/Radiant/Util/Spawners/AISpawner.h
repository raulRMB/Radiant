// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

UCLASS()
class RADIANT_API AAISpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARTAICharacter> AI;

	UPROPERTY(EditAnywhere)
	float SpawnRange;

	UPROPERTY(EditAnywhere)
	float SpawnRate;
	
	UPROPERTY(EditAnywhere)
	int MaxSpawned = 20;

	int Count;

	void Spawn();

	UFUNCTION()
	void OnDeath();


public:	
	AAISpawner();

	void StartSpawning();
protected:
	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
