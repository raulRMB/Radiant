// Copyright Radiant Studios


#include "GAS/Abilities/BasicAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Player/Avatar.h"
#include "GAS/Tasks/PlayMontageAndWaitForEvent.h"
#include "Util/Util.h"

void UBasicAttack::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimCancelled(EventTag, EventData);
}

void UBasicAttack::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimCompleted(EventTag, EventData);
}

void UBasicAttack::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimInterrupted(EventTag, EventData);
}

void UBasicAttack::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
	Super::OnAnimBlendOut(EventTag, EventData);
}

void UBasicAttack::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);

	if(HasAuthority(&CurrentActivationInfo))
	{
		AAvatar* Hero = Cast<AAvatar>(GetAvatarActorFromActorInfo());
		FTransform Transform = FTransform(Hero->GetMesh()->GetSocketLocation(FName("LeftHandSocket")));
		AHeatSeeking* Projectile = GetWorld()->SpawnActorDeferred<AHeatSeeking>(ProjectileClass, Transform, GetOwningActorFromActorInfo(), Hero);
		int TargetId = Hero->GetPlayerState<ARTPlayerState>()->GetTargetId();
		AAvatar* Target = UUtil::GetHeroFromPlayerID(this, TargetId);
		Projectile->SetTarget(Target);
		Projectile->FinishSpawning(Transform);
	}
	
	ReturnToDefault();
}

void UBasicAttack::OnUncancellableEventRecieved(FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Uncancellable"));
}

void UBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo());
	ARTPlayerState* Owner = Cast<ARTPlayerState>(GetOwningActorFromActorInfo());

	SetSelfTags(true);

	AAvatar* Target = UUtil::GetHeroFromPlayerID(this, Avatar->GetPlayerState<ARTPlayerState>()->GetTargetId());
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

	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.Uncancellable"));
	WaitEvent->EventReceived.AddDynamic(this, &UBasicAttack::OnUncancellableEventRecieved);
	WaitEvent->ReadyForActivation();
	
	BindAnimations();
}
