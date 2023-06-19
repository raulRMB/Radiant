// Copyright Radiant Studios


#include "GAS/Tasks/AbilityTask_PathTo.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

UAbilityTask_PathTo::UAbilityTask_PathTo()
{
	bTickingTask = true;
}

UAbilityTask_PathTo* UAbilityTask_PathTo::PathTo(UGameplayAbility* OwningAbility, FName TaskInstanceName,
                                                 FHitResult HitResult)
{
	UAbilityTask_PathTo* MyTask = NewAbilityTask<UAbilityTask_PathTo>(OwningAbility, TaskInstanceName);

	MyTask->Target = HitResult.GetActor();
	MyTask->Location = HitResult.Location;
	MyTask->bFollow = static_cast<bool>(Cast<AAvatar>(HitResult.GetActor()));

	return MyTask;
}

void UAbilityTask_PathTo::Activate()
{
	Super::Activate();
}

void UAbilityTask_PathTo::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(bFollow && !Target)
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnPathToComplete.Broadcast(false);
		}
		EndTask();
		return;
	}
	
	if(Ability)
	{
		if(AAvatar* Avatar = Cast<AAvatar>(Ability->GetAvatarActorFromActorInfo()))
		{
			if(Target)
			{
				float Dist = Avatar->GetDistanceTo(Target);
				if(Dist < Avatar->AttackRange)
				{
					if(Avatar->CheckShouldAttack())
						return;
				}
			}
			
			if(Avatar->HasTag("States.Movement.Stopped"))
				return;
			
			float Dist = FVector::Dist(Avatar->GetActorLocation(), Location);
			if(Dist < 190.f)
			{
				if(ShouldBroadcastAbilityTaskDelegates())
				{
					OnPathToComplete.Broadcast(true);
				}
				EndTask();
			}
			
			if(auto Controller = Avatar->GetController())
			{
				if(bFollow)
				{
					UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, Target);
				}
				else
				{
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Location);
				}
			}
		}
	}
}

void UAbilityTask_PathTo::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	
}

void UAbilityTask_PathTo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_PathTo, Target);
	DOREPLIFETIME(UAbilityTask_PathTo, bFollow);
	DOREPLIFETIME(UAbilityTask_PathTo, Location);
}
