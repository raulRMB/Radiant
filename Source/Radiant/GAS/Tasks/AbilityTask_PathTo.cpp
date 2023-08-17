// Copyright Radiant Studios


#include "GAS/Tasks/AbilityTask_PathTo.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Util/Util.h"

UAbilityTask_PathTo::UAbilityTask_PathTo()
{
	bTickingTask = true;
	bCanMove = true;
	bSimulatedTask = true;
}

UAbilityTask_PathTo* UAbilityTask_PathTo::PathTo(UGameplayAbility* OwningAbility, FName TaskInstanceName,
                                                 FHitResult HitResult)
{
	UAbilityTask_PathTo* MyTask = NewAbilityTask<UAbilityTask_PathTo>(OwningAbility, TaskInstanceName);

	MyTask->Target = HitResult.GetActor();
	MyTask->Location = HitResult.Location;
	MyTask->bFollow = static_cast<bool>(Cast<ITargetable>(HitResult.GetActor()));

	return MyTask;
}

void UAbilityTask_PathTo::OnUnitDied()
{
	if(AAvatar* Avatar = Cast<AAvatar>(Ability->GetAvatarActorFromActorInfo()))
	{
		if(AController* Controller = Avatar->GetController())
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Avatar->GetActorLocation());
		}
	}
	bCanMove = false;
	EndTask();
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		OnPathToComplete.Broadcast(false);
	}
}

void UAbilityTask_PathTo::Activate()
{
	Super::Activate();
	if(const AAvatar* Avatar = Cast<AAvatar>(GetAvatarActor()))
	{
		Avatar->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	if(IKillable* Killable = Cast<IKillable>(GetAvatarActor()))
	{
		if(Killable->GetIsDead())
		{
			EndTask();
			if(ShouldBroadcastAbilityTaskDelegates())
			{
				OnPathToComplete.Broadcast(false);
			}
			return;
		}
		Killable->OnUnitDied.AddUObject(this, &UAbilityTask_PathTo::OnUnitDied);
	}
	if(IKillable* KillableTarget = Cast<IKillable>(Target))
	{
		KillableTarget->OnUnitDied.AddUObject(this, &UAbilityTask_PathTo::OnUnitDied);
	}
}

void UAbilityTask_PathTo::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	if(!bCanMove)
	{
		EndTask();
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnPathToComplete.Broadcast(false);
		}
		return;	
	}
	
	if(bFollow && !IsValid(Target))
	{
		EndTask();
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnPathToComplete.Broadcast(false);
		}
		return;
	}
	
	if(Ability)
	{
		if(!Ability->IsActive())
		{			
			EndTask();
			if(ShouldBroadcastAbilityTaskDelegates())
			{
				OnPathToComplete.Broadcast(false);
			}
			return;
		}
			
		if(AAvatar* Avatar = Cast<AAvatar>(Ability->GetAvatarActorFromActorInfo()))
		{
			if(Target)
			{
				float Dist = Avatar->GetDistanceTo(Target);
				if(Dist < Avatar->AttributeSet->GetAttackRange())
				{
					if(Avatar->CheckShouldAttack())
					{
						return;
					}
				}
			}
			
			if(Avatar->HasTag("States.Movement.Stopped"))
			{
				return;
			}

			if(Avatar->HasTag("CC.Rooted"))
			{
				Avatar->GetCharacterMovement()->bOrientRotationToMovement = false;
				return;
			}
			if(Avatar->GetCharacterMovement()->bOrientRotationToMovement == false)
			{
				Avatar->GetCharacterMovement()->bOrientRotationToMovement = true;
			}
			
			float Dist = FVector::Dist(Avatar->GetActorLocation(), Location);
			if(Dist < 190.f)
			{
				EndTask();
				if(ShouldBroadcastAbilityTaskDelegates())
				{
					OnPathToComplete.Broadcast(true);
				}
				return;
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
	else
	{
		EndTask();
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnPathToComplete.Broadcast(false);
		}
	}
}

void UAbilityTask_PathTo::OnDestroy(bool AbilityEnded)
{
	if(IsValid(Ability))
	{
		if(Ability->GetAvatarActorFromActorInfo())
		{
			if(AAvatar* Avatar = Cast<AAvatar>(Ability->GetAvatarActorFromActorInfo()))
			{
				Avatar->GetCharacterMovement()->bOrientRotationToMovement = false;
				Avatar->CheckShouldAttack();
				if(AController* Controller = Avatar->GetController())
				{
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Avatar->GetActorLocation());
				}
			}
		}
	}
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_PathTo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_PathTo, Target);
	DOREPLIFETIME(UAbilityTask_PathTo, bFollow);
	DOREPLIFETIME(UAbilityTask_PathTo, Location);
}
