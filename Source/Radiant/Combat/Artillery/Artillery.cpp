// Copyright Radiant Studios


#include "Combat/Artillery/Artillery.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"

// Add default functionality here for any IArtillery functions that are not pure virtual.
bool AArtillery::ShouldHit(AActor* OtherActor)
{
	AAvatar* Target = Cast<AAvatar>(OtherActor);
	AAvatar* Self = Cast<AAvatar>(GetInstigator());
	ARTPlayerState* TargetState = Target->GetPlayerState<ARTPlayerState>();
	ARTPlayerState* MyState = Self->GetPlayerState<ARTPlayerState>();

	if(!Target || !Self || !TargetState || !MyState)
	{
		return false;
	}
	
	if(Behavior & static_cast<int32>(EArtilleryBehavior::HitSelf) && Target == Self)
	{
		return true;
	}
	if(Behavior & static_cast<int32>(EArtilleryBehavior::HitAllies) && TargetState->TeamId == MyState->TeamId && Target != Self)
	{
		return true;
	}
	if(Behavior & static_cast<int32>(EArtilleryBehavior::HitEnemies) && TargetState->TeamId != MyState->TeamId)
	{
		return true;
	}
	
	return false;
}
