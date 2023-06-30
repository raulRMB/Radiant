// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Util/Interfaces/Killable.h"
#include "Util/Interfaces/Targetable.h"
#include "Util/Interfaces/TeamMember.h"
#include "RTCharacter.generated.h"

UCLASS()
class RADIANT_API ARTCharacter : public ACharacter, public IKillable, public ITargetable, public ITeamMember, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(Replicated, EditAnywhere)
	ETeamId TeamId;
	
	UPROPERTY(EditAnywhere, Category=Death)
	TArray<TSubclassOf<class UGameplayAbility>> DeathAbilities;

protected:
	UPROPERTY()
	class URTAbilitySystemComponent* AbilitySystemComponent;
	
public:
	// Sets default values for this character's properties
	ARTCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual TArray<TSubclassOf<class UGameplayAbility>> GetDeathAbilities() const override;

	virtual ETeamId GetTeamId() const override { return TeamId; }

public:
	UFUNCTION(NetMulticast, Reliable)
	void M_SetIgnoreWalls(const bool bIgnoreWalls);
};
