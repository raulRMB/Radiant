// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Combat/Artillery/Artillery.h"
#include "Enums/EAOEApplicationType.h"
#include "AreaOfEffect.generated.h"

UCLASS()
class RADIANT_API AAreaOfEffect : public AArtillery
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EAOEApplicationType ApplicationType = EAOEApplicationType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true,
		EditConditionHides,
		EditCondition = "ApplicationType != EAOEApplicationType::Instant"))
	float LifeSpan = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true,
		EditConditionHides,
		EditCondition = "ApplicationType == EAOEApplicationType::OnTimerEnd_Constant"))
	float ConstantLifeSpan = 0.f;
	
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* TimerDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag CueTag;

	FVector CashedTimerScale;

public:
	AAreaOfEffect();
	
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	void ApplyGameplayEffectsToTargets();
	void ApplyGameplayEffectsToASC(UAbilitySystemComponent* ASC);
	void RemoveGameplayEffectsFromASC(UAbilitySystemComponent* ASC);
	
	UPROPERTY()
	class ARTCharacter* SourceCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<class ITeamMember*> EffectTargets;

public:
	virtual void Tick(float DeltaTime) override;

	void SetSourceCharacter(class ARTCharacter* NewCharacter) { SourceCharacter = NewCharacter; }
	void SetEffectTargets(TArray<ITeamMember*> NewEffectTargets) const { NewEffectTargets = EffectTargets; }

	UFUNCTION()
	void OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHitBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void ApplyInstantEffects();

	UFUNCTION()
	void OnTimerEnd();
};
