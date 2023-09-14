// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RTGameState.generated.h"

class AAvatar;
DECLARE_DELEGATE_TwoParams(FOnHeroDeath, uint32, uint32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FRadianiteChangedSignature, double XP, uint32 Level);
DECLARE_MULTICAST_DELEGATE(FTeamLevelupSignature);

UCLASS()
class RADIANT_API ARTGameState : public AGameState
{
	GENERATED_BODY()

public:
	ARTGameState();

	UPROPERTY(Replicated, VisibleInstanceOnly)
	uint32 BlueScore;
	UPROPERTY(Replicated, VisibleInstanceOnly)
	uint32 RedScore;
	
	UPROPERTY(Replicated)
	uint32 BlueLevel;
	UPROPERTY(Replicated)
	uint32 RedLevel;

	UPROPERTY(Replicated)
	uint32 BlueRadianite;
	UPROPERTY(Replicated)
	uint32 RedRadianite;

	double PreviousBlueNextLevel = 0;
	double BlueUntilNextLevel = 10;
	double PreviousRedNextLevel = 0;
	double RedUntilNextLevel = 10;

	UPROPERTY(EditAnywhere)
	double RadianiteToLevelMultiplier = 1.5f;
	
	FRadianiteChangedSignature OnRedRadianiteChangedDelegate;
	FRadianiteChangedSignature OnBlueRadianiteChangedDelegate;

	UFUNCTION(Server, Reliable)
	void OnHeroDeath(AAvatar* Hero);

	UFUNCTION(Server, Reliable)
	void DepositRadianite(ETeamId TeamId, uint32 Radianite);
	
	UFUNCTION(NetMulticast, Reliable)
	void NotifyHeroDeath(uint32 Score1, uint32 Score2);

	UFUNCTION(NetMulticast, Reliable)
	void NotifyRadianiteChanged(ETeamId TeamId, double XP, uint32 Level);

	FOnHeroDeath OnHeroDeathDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int32 GetTeamLevel(ETeamId TeamId) const;

protected:
	virtual void BeginPlay() override;
};
