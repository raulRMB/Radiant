// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaOfEffect.generated.h"

UCLASS()
class RADIANT_API AAreaOfEffect : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	float LifeSpan = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> GameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
public:	
	// Sets default values for this actor's properties
	AAreaOfEffect();

	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void ApplyGameplayEffects();
	class AHero* Avatar;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<class AHero*> EffectTargets;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetAvatar(class AHero* NewAvatar) { Avatar = NewAvatar; }
	void SetEffectTargets(TArray<AHero*> NewEffectTargets) const { NewEffectTargets = EffectTargets; }

	void ApplyInstantEffects();
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
};
