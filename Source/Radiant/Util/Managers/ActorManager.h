// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActorManager.generated.h"

enum class ETeamId : uint8;
DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayersUpdatedSignature, class AAvatar*, bool /*bRemoved*/);


UCLASS()
class RADIANT_API UActorManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<class AAvatar*> Players;


	/* TEAM STUFF */
	UPROPERTY()
	TMap<ETeamId, class ATeamGridManager*> TeamGridManagers;
	
public:
	FPlayersUpdatedSignature OnPlayersUpdated;
	
	void GetAllPlayers(TArray<class AAvatar*>& OutPlayers) const;
	void AddPlayer(class AAvatar* Player);
	void RemovePlayer(class AAvatar* Player);

	class ATeamGridManager* GetTeamGridManager(ETeamId TeamId) const;
	void AddTeamGridManager(ETeamId TeamId, ATeamGridManager* TeamGridManager);
};
