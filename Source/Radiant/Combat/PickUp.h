// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

DECLARE_DELEGATE(FPickedupSignature)

UCLASS()
class RADIANT_API APickUp : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> GameplayEffect;

public:	
	APickUp();
	
	FPickedupSignature OnPickedUp;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
