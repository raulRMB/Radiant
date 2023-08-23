// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapHitBox.generated.h"

class ARTCharacter;

UCLASS()
class RADIANT_API AMapHitBox : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	TArray<ARTCharacter*> Characters;

	UPROPERTY(EditAnywhere)
	bool bBlockCharacters = true;
	
public:
	// Sets default values for this actor's properties
	AMapHitBox();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
