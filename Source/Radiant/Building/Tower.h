// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Building.h"
#include "GameFramework/Actor.h"
#include "Util/Interfaces/TeamMember.h"
#include "Tower.generated.h"

UCLASS()
class RADIANT_API ATower : public ABuilding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stats, meta=(AllowPrivateAccess=true))
	float AttackDamage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats, meta=(AllowPrivateAccess=true, MakeEditWidget))
	FVector GemOffset = FVector(0.f, 0.f, 0.f);

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	// class UStaticMeshComponent* BaseMesh;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	// class UStaticMeshComponent* Gem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class USphereComponent* AttackRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess=true))
	TObjectPtr<class AActor> Target;

public:
	ATower();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	TObjectPtr<class AActor> GetTarget() const;

private:
	UFUNCTION()
	void BeingOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	                  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                  const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	                class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
