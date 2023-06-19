// Copyright Radiant Studios


#include "GAS/Abilities/GAMove.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GAS/Tasks/AbilityTask_PathTo.h"
#include "Player/Avatar.h"

UGAMove::UGAMove()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Ability.PathTo"));
	AbilityTriggers.Add(TriggerData);
}

void UGAMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData,0);
	
	UAbilityTask_PathTo* Task = UAbilityTask_PathTo::PathTo(this, FName("MoveTask"), HitResult);
	Task->OnPathToComplete.AddUObject(this, &UGAMove::OnTargetLocationReached);
	Task->ReadyForActivation();
}

void UGAMove::OnTargetLocationReached(bool bGoalReached)
{
	if(AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo()))
	{
		Avatar->StopMovement();
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
