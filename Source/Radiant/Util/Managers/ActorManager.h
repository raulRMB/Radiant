// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActorManager.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayersUpdatedSignature, class AAvatar*, bool /*bRemoved*/);

/**
 * 
 */
UCLASS()
class RADIANT_API UActorManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	TArray<class AAvatar*> Players;
public:
	FPlayersUpdatedSignature OnPlayersUpdated;
	
	void GetAllPlayers(TArray<class AAvatar*>& OutPlayers) const;
	void AddPlayer(class AAvatar* Player);
	void RemovePlayer(class AAvatar* Player);
};
