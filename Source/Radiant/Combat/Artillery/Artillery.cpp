// Copyright Radiant Studios


#include "Combat/Artillery/Artillery.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"

// Add default functionality here for any IArtillery functions that are not pure virtual.
bool AArtillery::ShouldHit(AActor* OtherActor)
{
	ITeamMember* Self = Cast<ITeamMember>(Owner);
	ITeamMember* Target = Cast<ITeamMember>(OtherActor);

	if(!Self || !Target)
	{
		return false;
	}
	
	if(Behavior & static_cast<int32>(EArtilleryBehavior::HitSelf) && OtherActor == Owner)
	{
		return true;
	}
	if(Behavior & static_cast<int32>(EArtilleryBehavior::HitAllies) && Self->GetTeamId() == Target->GetTeamId() && OtherActor != Owner)
	{
		return true;
	}
	if(Behavior & static_cast<int32>(EArtilleryBehavior::HitEnemies) && Self->GetTeamId() != Target->GetTeamId())
	{
		return true;
	}
	
	return false;
}
