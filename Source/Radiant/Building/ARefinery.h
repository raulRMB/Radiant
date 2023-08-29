// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Base.h"
#include "ARefinery.generated.h"

UCLASS()
class RADIANT_API AARefinery : public ABase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	UPROPERTY()
	class ARTGameState* GameState;
	
public:
	AARefinery();
	
private:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
