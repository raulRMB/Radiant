// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Combat/Artillery/Artillery.h"
#include "LinearSkillshot.generated.h"

UCLASS()
class RADIANT_API ALinearSkillshot : public AArtillery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTag HitCueTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability", meta = (AllowPrivateAccess = "true"))
	uint8 bConsumeOnHit : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float Range = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float LifeSpan = 1.f;

	UPROPERTY()
	TArray<AActor*> AffectedActors;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	uint8 bUseRange : 1;

	// Sets default values for this actor's properties
	ALinearSkillshot();

	void SetGameplayEffects(TArray<TSubclassOf<class UGameplayEffect>> NewGameplayEffects);
	void SetMovementSpeed(const float NewSpeed) { Speed = NewSpeed; }
	void SetRange(const float NewRange) { Range = NewRange; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                          const FHitResult& SweepResult)
	{
	}

	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
	{
	}

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
};
