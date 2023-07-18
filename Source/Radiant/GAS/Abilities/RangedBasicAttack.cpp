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
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimCancelled(EventTag, EventData);
}

void URangedBasicAttack::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimCompleted(EventTag, EventData);
}

void URangedBasicAttack::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimInterrupted(EventTag, EventData);
}

void URangedBasicAttack::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimBlendOut(EventTag, EventData);
}

void URangedBasicAttack::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);

	if(HasAuthority(&CurrentActivationInfo))
	{
		AAvatar* Hero = Cast<AAvatar>(GetAvatarActorFromActorInfo());
		FTransform Transform = FTransform(Hero->GetMesh()->GetSocketLocation(FName("LeftHandSocket")));
		AHeatSeeking* Projectile = GetWorld()->SpawnActorDeferred<AHeatSeeking>(ProjectileClass, Transform, GetOwningActorFromActorInfo(), Hero);
		AActor* Target = Hero->GetPlayerState<ARTPlayerState>()->GetTarget();
		Projectile->SetTarget(Target);
		Projectile->FinishSpawning(Transform);
	}
	
	ReturnToDefault();
}

void URangedBasicAttack::OnUncancellableEventRecieved(FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
}

void URangedBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo());
	
	SetSelfTags(true);
	
	AActor* Target = Avatar->GetPlayerState<ARTPlayerState>()->GetTarget();
	if(!Target)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	RTLOGP("Target %s", *Target->GetName())
	RTPRINTP("Target %s", *Target->GetName())
	
	FVector Loc = Avatar->GetActorLocation();
	Loc.Z = 0;
	FVector Direction = (Target->GetActorLocation() - Loc).GetSafeNormal();
	Avatar->SetRotationLock(true, Direction);

	SetMouseWorldLocation(Target->GetActorLocation());

	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.Uncancellable"));
	WaitEvent->EventReceived.AddDynamic(this, &URangedBasicAttack::OnUncancellableEventRecieved);
	WaitEvent->ReadyForActivation();

	float AttackSpeed = Cast<ARTPlayerState>(GetOwningActorFromActorInfo())->GetAttributeSetBase()->GetAttackSpeed();
	BindAnimations(AttackSpeed);
}
