// Copyright Radiant Studios


#include "GAS/Abilities/Building/GABuildAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Tasks/AbilityTask_BuildMode.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerController.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

UGABuildAbility::UGABuildAbility()
{
	bIsCancelable = false;
}

void UGABuildAbility::OnOrderAccepted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	if(AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo()))
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
}

void UGABuildAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_BuildMode* Task = UAbilityTask_BuildMode::BuildModeTask(this, FName("BuildModeTask"), EnvironmentType);
	Task->ReadyForActivation();

	if(ARTPlayerState* PS = Cast<ARTPlayerState>(GetOwningActorFromActorInfo()))
	{
		if(ARTPlayerController* PC = Cast<ARTPlayerController>(PS->GetPlayerController()))
		{
			PC->OrderAccepted.AddUObject(this, &UGABuildAbility::OnOrderAccepted);
		}
	}
}
