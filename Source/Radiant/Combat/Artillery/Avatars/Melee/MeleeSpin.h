// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Combat/Artillery/AreaOfEffect.h"
#include "MeleeSpin.generated.h"

UCLASS()
class RADIANT_API AMeleeSpin : public AAreaOfEffect
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* OuterTornadoMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* InnerTornadoMesh;

	UPROPERTY(EditAnywhere)
	double OuterSpinSpeed = 1.;

	UPROPERTY(EditAnywhere)
	double InnerSpinSpeed = .5;

public:
	// Sets default values for this actor's properties
	AMeleeSpin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
