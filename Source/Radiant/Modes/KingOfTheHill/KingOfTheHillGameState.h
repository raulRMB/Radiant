// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Modes/Base/RTGameState.h"
#include "Enums/TeamId.h"
#include "KingOfTheHillGameState.generated.h"


UCLASS()
class RADIANT_API AKingOfTheHillGameState : public ARTGameState
{
	GENERATED_BODY()
	
	TObjectPtr<class ACaptureArea> CaptureArea;
	TObjectPtr<class UCaptureAreaBar> CaptureAreaBar;

	float CaptureAreaPercent = 0.5f;

	UPROPERTY(Replicated)
	ETeamId CaptureAreaTeam = ETeamId::Neutral;
	UPROPERTY(Replicated)
	uint8 RedTeamInArea = 0;
	UPROPERTY(Replicated)
	uint8 BlueTeamInArea = 0;
	UPROPERTY(Replicated)
	uint8 bGameOver : 1;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	float GetCaptureAreaPercent() const { return CaptureAreaPercent; }

	void MatchEnded();
private:
	UFUNCTION()
	void OnCaptureAreaUpdate(ETeamId TeamId, bool bEntered);

	void UpdateCaptureAreaPercent(float DeltaSeconds);
};
