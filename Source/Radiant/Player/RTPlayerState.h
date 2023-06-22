// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Util/Interfaces/TeamMember.h"
#include "RTPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARTPlayerState : public APlayerState, public IAbilitySystemInterface, public ITeamMember
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class URTHeroAttributeSetBase* AttributeSetBase;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(Replicated, VisibleAnywhere)
	int TargetId = -1;
	UPROPERTY(Replicated, VisibleAnywhere)
	ETeamId TeamId = ETeamId::Neutral;
	UPROPERTY(ReplicatedUsing=OnRep_UsernameChanged, VisibleAnywhere)
	FString Username = "";
public:
	ARTPlayerState();

	UFUNCTION(BlueprintCallable)
	void SetPlayerStats();

	UFUNCTION(Server, Reliable)
	void S_SetTargetId(const int NewTargetId);

	UFUNCTION()
	virtual void OnRep_UsernameChanged();
	
	int GetTargetId() const { return TargetId; }
	virtual ETeamId GetTeamId() const override { return TeamId; }

	void SetTeamId(ETeamId NewTeamId) { TeamId = NewTeamId; }
	
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	class URTHeroAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(Server, Reliable)
	void SetUsername(const FString& String);
	FString GetUsername();
};
