// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Interfaces/TeamMember.h"
#include "Tower.generated.h"

UCLASS()
class RADIANT_API ATower : public AActor, public IAbilitySystemInterface, public ITeamMember
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stats, meta=(AllowPrivateAccess=true))
	float MaxHealth = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stats, meta=(AllowPrivateAccess=true))
	float AttackDamage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats, meta=(AllowPrivateAccess=true, MakeEditWidget))
	FVector GemOffset = FVector(0.f, 0.f, 0.f);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Gem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class USphereComponent* AttackRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class UTowerAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess=true))
	TObjectPtr<class AActor> Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Team, meta=(AllowPrivateAccess=true))
	int32 TeamId = -1;
public:	
	// Sets default values for this actor's properties
	ATower();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTowerAttributeSet* GetAttributeSet() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FTransform GetGemTransform() const;
	TObjectPtr<class AActor> GetTarget() const;

	virtual int32 GetTeamId() const override { return TeamId; }
private:
	UFUNCTION()
	void BeingOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	                  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                  const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	                class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void GiveInitialAbilities();
};
