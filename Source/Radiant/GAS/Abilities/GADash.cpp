// Copyright Radiant Studios


#include "GAS/Abilities/GADash.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Character/Hero.h"
#include "GAS/Tasks/MoveToLocationParabolic.h"

void UGADash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	switch (DashType)
	{
	case EDashType::Linear:
		LinearDash();
		break;
	case EDashType::Parabolic:
		ParabolicDash();
		break;
	case EDashType::TeleportInstant:
		TeleportInstant();
		break;
	default:
		break;
	}
}

void UGADash::LinearDash()
{
	UAbilityTask_ApplyRootMotionMoveToForce* MoveTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		FName("MoveTask"),
		GetRangedBaseMouseLocationWithHeroHalfHeight(),
		DashTime,
		false,
		EMovementMode::MOVE_Custom,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		false);

	MoveTask->OnTimedOut.AddDynamic(this, &UGADash::OnTargetLocationReached);
	MoveTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UGADash::OnTargetLocationReached);
	MoveTask->ReadyForActivation();
}

void UGADash::ParabolicDash()
{
	UMoveToLocationParabolic* MoveTask = UMoveToLocationParabolic::MoveToLocationParabolic(
		this,
		FName("MoveTask"),
		GetRangedBaseMouseLocationWithHeroHalfHeight(),
		DashTime,
		nullptr,
		nullptr,
		ParabolicCurve,
		CurveMultiplier);

	MoveTask->OnTargetLocationReached.AddDynamic(this, &UGADash::OnTargetLocationReached);
	MoveTask->ReadyForActivation();
}

void UGADash::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);
	if(DashType == EDashType::TeleportTimed)
		TeleportInstant();
}


void UGADash::OnTargetLocationReached()
{
	ReturnToDefaultAndEndAbility(false);
}

void UGADash::TeleportInstant()
{
	if(HasAuthority(&CurrentActivationInfo))
	{
		GetAvatarActorFromActorInfo()->SetActorLocation(GetRangedBaseMouseLocationWithHeroHalfHeight());
		OnTargetLocationReached();
	}
}
