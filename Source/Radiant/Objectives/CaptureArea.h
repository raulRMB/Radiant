// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Util/Enums/TeamId.h"
#include "CaptureArea.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdatePlayersInAreaSignature, ETeamId, bool);

UCLASS()
class RADIANT_API ACaptureArea : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta = (AllowPrivateAccess=true))
	class UBoxComponent* HitBox;
public:	
	// Sets default values for this actor's properties
	ACaptureArea();

	FOnUpdatePlayersInAreaSignature OnUpdatePlayersInArea;
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
