// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Util/Enums/TeamId.h"
#include "RTGameState.generated.h"

DECLARE_DELEGATE_TwoParams(FOnHeroDeath, uint32, uint32);

USTRUCT()
struct FTeam
{
	GENERATED_BODY()
	
	uint8 TeamID : 1;
	int Score;
	TArray<class AAvatar*> Heroes;
};

/**
 * 
 */
UCLASS()
class RADIANT_API ARTGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FTeam RedTeam;
	UPROPERTY(VisibleAnywhere)
	FTeam BlueTeam;

	TObjectPtr<class ACaptureArea> CaptureArea;
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;

	float CaptureAreaPercent = 0.5f;

	UPROPERTY(Replicated)
	ETeamId CaptureAreaTeam = ETeamId::Neutral;
	UPROPERTY(Replicated)
	uint8 RedTeamInArea = 0;
	UPROPERTY(Replicated)
	uint8 BlueTeamInArea = 0;
public:
	ARTGameState();
	
	UPROPERTY(Replicated, VisibleInstanceOnly)
	uint32 RedScore;
	UPROPERTY(Replicated, VisibleInstanceOnly)
	uint32 BlueScore;
	
	UFUNCTION(Server, Reliable)
	void OnHeroDeath(AAvatar* Hero);

	UFUNCTION(NetMulticast, Reliable)
	void NotifyHeroDeath(uint32 Score1, uint32 Score2);
	
	FOnHeroDeath OnHeroDeathDelegate;
	
	virtual void GetLifetimeReplicatedProps (TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void OnCaptureAreaUpdate(ETeamId TeamId, bool bEntered);

	void UpdateCaptureAreaPercent(float DeltaTime);
};
