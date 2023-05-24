// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "RadiantPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARadiantPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_PlayerID, VisibleAnywhere)
	int PlayerID;

	UFUNCTION()
	virtual void OnRep_PlayerID();	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int GetPlayerID() const { return PlayerID; }
	void SetPlayerID(const int ID) { PlayerID = ID; }
}; 
