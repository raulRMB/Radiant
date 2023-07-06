// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "GameFramework/Actor.h"
#include "PickUpSpawner.generated.h"

UCLASS()
class RADIANT_API APickUpSpawner : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	TSubclassOf<APickUp> PickUpClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	int32 SpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	int32 InitialDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true, MakeEditWidget))
	FVector SpawnOffset;
public:	
	APickUpSpawner();

protected:
	UFUNCTION()
	void OnPickedUp();
	void SpawnPickup();
	
	virtual void BeginPlay() override;
};
