// Copyright Radiant Studios


#include "Util/Managers/ActorManager.h"

void UActorManager::GetAllPlayers(TArray<AAvatar*>& OutPlayers) const
{
	OutPlayers = Players;
}

void UActorManager::AddPlayer(AAvatar* Player)
{
	Players.Add(Player);
	OnPlayersUpdated.Broadcast(Player, false);
}

void UActorManager::RemovePlayer(AAvatar* Player)
{
	Players.Remove(Player);
	OnPlayersUpdated.Broadcast(Player, true);
}
