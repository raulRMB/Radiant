// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat/Artillery/Artillery.h"
#include "HeatSeeking.generated.h"

UCLASS()
class RADIANT_API AHeatSeeking : public AArtillery
{
	GENERATED_BODY()

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true), Category = "Targeting")
	AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Collision")
	class USphereComponent* HitBox;
	
	UPROPERTY(EditAnywhere)
	float Speed = 10.f;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTarget(AActor* NewTarget);

	FGameplayEffectSpecHandle EffectSpecHandle;
public:
	
	// Sets default values for this actor's properties
	AHeatSeeking();

	FGameplayEffectSpecHandle GetEffectSpecHandle() const { return EffectSpecHandle; }
	void SetEffectSpecHandle(FGameplayEffectSpecHandle NewHandle) { EffectSpecHandle = NewHandle; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnOvelapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTarget(AActor* NewTarget) { Target = NewTarget; }
};
