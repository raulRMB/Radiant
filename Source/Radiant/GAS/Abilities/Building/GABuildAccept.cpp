// Copyright Radiant Studios


#include "GAS/Abilities/Building/GABuildAccept.h"

#include "Player/Avatar.h"
#include "Util/Util.h"

void UGABuildAccept::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(AAvatar* Avatar = GetAvatar())
	{
		FGridPiece GridPiece;
		GridPiece.TeamId = Cast<ITeamMember>(GetOwningActorFromActorInfo())->GetTeamId();
		GridPiece.Type = EnvironmentType;
		FVector Mouse = UUtil::GetMousePosition(this, {});
		Mouse.X = FMath::RoundToInt(Mouse.X / 200);
		Mouse.Y = FMath::RoundToInt(Mouse.Y / 200);
		GridPiece.Position = FIntVector2(Mouse.X, Mouse.Y);
		Avatar->S_PlaceGridPiece(GridPiece);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
