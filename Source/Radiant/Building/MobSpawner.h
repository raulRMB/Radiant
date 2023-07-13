// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "MobSpawner.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API AMobSpawner : public ABuilding
{
	GENERATED_BODY()
public:
	AMobSpawner();
private:
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<class ARTAICharacter> MobClass;
	
	UPROPERTY(EditAnywhere)
	float SpawnRange;

	UPROPERTY(EditAnywhere)
	float SpawnRate;
	
	UPROPERTY(EditAnywhere)
	int MaxSpawned = 20;

	UPROPERTY(EditAnywhere)
	float MinRange;
	
	int Count;

	void Spawn();

	UFUNCTION()
	void OnDeath();
	void StartSpawning();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	FTimerHandle Handle;
};
