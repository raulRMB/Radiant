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
protected:
	UPROPERTY()
	class URTAbilitySystemComponent* AbilitySystemComponent;

public:
	// Sets default values for this character's properties
	ARTCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual ETeamId GetTeamId() const override { return TeamId; }
	UFUNCTION()
	void OnDeath();
public:
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
