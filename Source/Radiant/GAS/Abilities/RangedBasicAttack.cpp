// Copyright Radiant Studios

#include "GAS/Abilities/RangedBasicAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Player/Avatar.h"
#include "GAS/Tasks/PlayMontageAndWaitForEvent.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

void URangedBasicAttack::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimCancelled(EventTag, EventData);
}

void URangedBasicAttack::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimCompleted(EventTag, EventData);
}

void URangedBasicAttack::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimInterrupted(EventTag, EventData);
}

void URangedBasicAttack::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimBlendOut(EventTag, EventData);
}

void URangedBasicAttack::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);

	if(HasAuthority(&CurrentActivationInfo))
	{
		AAvatar* Hero = Cast<AAvatar>(GetAvatarActorFromActorInfo());
		FTransform Transform = FTransform(Hero->GetMesh()->GetSocketLocation(SocketName));
		AHeatSeeking* Projectile = GetWorld()->SpawnActorDeferred<AHeatSeeking>(ProjectileClass, Transform, GetOwningActorFromActorInfo(), Hero);
		Projectile->SetTarget(Target);
		Projectile->FinishSpawning(Transform);
	}
	
	ReturnToDefault();
}

void URangedBasicAttack::OnSetUnancelable(FGameplayEventData EventData)
{
	bIsCancelable = false;
}

void URangedBasicAttack::OnUnsetUncancelable(FGameplayEventData EventData)
{
	bIsCancelable = true;
}

void URangedBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	URTAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo());
	
	Target = Avatar->GetPlayerState<ARTPlayerState>()->GetTarget();
	if(!Target)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
		
	FVector Loc = Avatar->GetActorLocation();
	Loc.Z = 0;
	FVector Direction = (Target->GetActorLocation() - Loc).GetSafeNormal();
	Avatar->SetRotationLock(true, Direction);

	SetMouseWorldLocation(Target->GetActorLocation());

	float AttackSpeed = Cast<ARTPlayerState>(GetOwningActorFromActorInfo())->GetAttributeSet()->GetAttackSpeed();
	BindAnimations(AttackSpeed);

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.State.Uncancelable.Set"));
	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);
	Task->EventReceived.AddDynamic(this, &URangedBasicAttack::OnSetUnancelable);
	Task->Activate();

	EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.State.Uncancelable.Unset"));
	UAbilityTask_WaitGameplayEvent* Task2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);
	Task2->EventReceived.AddDynamic(this, &URangedBasicAttack::OnUnsetUncancelable);
	Task2->Activate();
}
