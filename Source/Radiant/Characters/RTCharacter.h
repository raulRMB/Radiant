// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Util/Interfaces/Hooker.h"
#include "Util/Interfaces/Killable.h"
#include "Util/Interfaces/Targetable.h"
#include "Util/Interfaces/TeamMember.h"
#include "Util/Managers/ActorManager.h"
#include "Components/StaticMeshComponent.h"
#include "RTCharacter.generated.h"

UCLASS()
class RADIANT_API ARTCharacter : public ACharacter, public IKillable, public ITargetable, public ITeamMember,
                                 public IAbilitySystemInterface, public IHooker
{
	GENERATED_BODY()

	UPROPERTY(Replicated, EditAnywhere)
	ETeamId TeamId;

	UPROPERTY(Replicated, EditAnywhere)
	uint8 bIsDead : 1;

	UPROPERTY()
	TArray<AActor*> ActorsInVisionRadius;

	UPROPERTY()
	TSet<AActor*> ActorsInVision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vision, meta = (AllowPrivateAccess = true))
	class USphereComponent* VisionRadius;

	UPROPERTY()
	UActorManager* ActorManager;

private:
	UFUNCTION()
	void RemoveActorInVision(AActor* Actor);
	UFUNCTION()
	void OnVisionBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	                          class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                          const struct FHitResult& SweepResult);
	UFUNCTION()
	void OnVisionEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	                        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	UPROPERTY()
	class URTAbilitySystemComponent* AbilitySystemComponent;
	void SetupVisionRadius() const;

	UPROPERTY(Replicated)
	class ATeamGridManager* TeamGridManager;

public:
	// Sets default values for this character's properties
	ARTCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual ETeamId GetTeamId() const override { return TeamId; }
	UFUNCTION()
	void OnDeath();

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	void CheckCanSeeTarget(AActor* Target);

	UActorManager* GetActorManager() const { return ActorManager; }
public:
	virtual void Tick(float DeltaSeconds);
	
	UFUNCTION(NetMulticast, Reliable)
	void M_SetIgnoreWalls(const bool bIgnoreWalls);

	virtual bool GetIsDead() const override { return bIsDead; }
	virtual void SetIsDead(const bool NewIsDead) override;
	UFUNCTION(NetMulticast, Reliable)
	void M_NotifyOnDeath();
	
	UPROPERTY(EditAnywhere)
	USoundBase* DeathSound;

	virtual FHookSocketInfo GetHookSocketInfo() const override;
};
