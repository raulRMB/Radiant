// Copyright Radiant Studios


#include "GAS/Abilities/Building/GABuildAccept.h"

#include "Kismet/GameplayStatics.h"
#include "Player/Avatar.h"
#include "Player/InventoryComponent.h"
#include "Util/Util.h"

void UGABuildAccept::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	if(!HasAuthority(&ActivationInfo))
	{
		if(AAvatar* Avatar = GetAvatar())
		{
			FGridPiece GridPiece;
			GridPiece.TeamId = Cast<ITeamMember>(GetOwningActorFromActorInfo())->GetTeamId();
			GridPiece.Type = EnvironmentType;
			FVector Mouse = UUtil::GetMousePosition(this, {});
			Mouse.X = FMath::RoundToInt(Mouse.X / 200);
			Mouse.Y = FMath::RoundToInt(Mouse.Y / 200);
			GridPiece.Position = FIntVector2(Mouse.X, Mouse.Y);

			Avatar->S_TryPlaceGridPiece(GridPiece, ItemName);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
