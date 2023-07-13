// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Combat/Artillery/Artillery.h"
#include "AreaOfEffect.generated.h"

UCLASS()
class RADIANT_API AAreaOfEffect : public AArtillery
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	float LifeSpan = 1.f;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* TimerDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTag CueTag;

	FVector CashedTimerScale;

public:
	AAreaOfEffect();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	void ApplyGameplayEffects();
	UPROPERTY()
	class ARTCharacter* SourceCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<class ITeamMember*> EffectTargets;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetSourceCharacter(class ARTCharacter* NewChracter) { SourceCharacter = NewChracter; }
	void SetEffectTargets(TArray<ITeamMember*> NewEffectTargets) const { NewEffectTargets = EffectTargets; }

	void ApplyInstantEffects();
};
