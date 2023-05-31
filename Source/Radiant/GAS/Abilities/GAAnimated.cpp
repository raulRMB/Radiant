// Copyright Radiant Studios


#include "GAS/Abilities/GAAnimated.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Hero.h"
#include "Combat/Artillery/LinearSkillshot.h"
#include "Player/RTPlayerState.h"
#include "Util/PlayMontageAndWaitForEvent.h"
#include "Util/Util.h"


UGAAnimated::UGAAnimated()
{
}

void UGAAnimated::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AHero* Avatar = Cast<AHero>(GetAvatarActorFromActorInfo());
	ARTPlayerState* Owner = Cast<ARTPlayerState>(GetOwningActorFromActorInfo());
	
	if(Avatar->GetLocalRole() == ROLE_AutonomousProxy)
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTags(SelfTags);
		GetAbilitySystemComponentFromActorInfo()->AddReplicatedLooseGameplayTags(SelfTags);
	}
	
	FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData,0);

	FVector Loc = Avatar->GetActorLocation();
	Loc.Z = 0;
	FVector Direction = (HitResult.Location - Loc).GetSafeNormal();
	Avatar->SetRotationLock(true, Direction);

	MouseWorldLocation = HitResult.Location;
	
	UPlayMontageAndWaitForEvent* Task = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, Montage,SpawnTags, 1.0f, NAME_None, false, 1.0f);
	Task->OnCompleted.AddDynamic(this, &UGAAnimated::OnCompleted);
	Task->OnCancelled.AddDynamic(this, &UGAAnimated::OnCancelled);
	Task->OnInterrupted.AddDynamic(this, &UGAAnimated::OnInterrupted);
	Task->OnBlendOut.AddDynamic(this, &UGAAnimated::OnBlendOut);
	Task->EventReceived.AddDynamic(this, &UGAAnimated::OnEventReceived);
	Task->ReadyForActivation();
}

void UGAAnimated::ReturnToDefaultAndEndAbility(bool bWasCancelled)
{
	AHero* Avatar = Cast<AHero>(GetAvatarActorFromActorInfo());
	Avatar->SetRotationLock(false);

	FGameplayTagContainer Tags;
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);

	if(Avatar->GetLocalRole() == ROLE_AutonomousProxy)
	{
		for(auto SelfTag : SelfTags)
		{
			if(Tags.HasTag(SelfTag))
			{
				GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(SelfTag);
				GetAbilitySystemComponentFromActorInfo()->RemoveReplicatedLooseGameplayTag(SelfTag);
			}
		}
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void UGAAnimated::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimCompleted(EventTag, EventData);
	ReturnToDefaultAndEndAbility();
}

void UGAAnimated::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimCancelled(EventTag, EventData);
	ReturnToDefaultAndEndAbility(true);
}

void UGAAnimated::OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimInterrupted(EventTag, EventData);
	ReturnToDefaultAndEndAbility(true);
}

void UGAAnimated::OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimBlendOut(EventTag, EventData);
	ReturnToDefaultAndEndAbility();
}

void UGAAnimated::OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimEventReceived(EventTag, EventData);
}