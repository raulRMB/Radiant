// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureArea.generated.h"

UCLASS()
class RADIANT_API ACaptureArea : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 TeamOneCount = 0;

	UPROPERTY(EditAnywhere)
	int32 TeamZeroCount = 0;

	UPROPERTY(VisibleAnywhere)
	int32 TeamPoints = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta = (AllowPrivateAccess=true))
	class UBoxComponent* HitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta = (AllowPrivateAccess=true))
	class UTextRenderComponent* PointsText;

	FTimerHandle TimerHandle;

	
public:	
	// Sets default values for this actor's properties
	ACaptureArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void BeingOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
					  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
					  const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
					class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckCapture();
};
