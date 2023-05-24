// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TargetedProjectile.generated.h"

UCLASS()
class RADIANT_API ATargetedProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true), Category = "Targeting")
	AActor* Target;

	UPROPERTY(EditAnywhere)
	float Speed = 10.f;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTarget(AActor* NewTarget);
public:
	
	// Sets default values for this actor's properties
	ATargetedProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
