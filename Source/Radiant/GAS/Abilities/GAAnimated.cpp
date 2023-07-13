// Copyright Radiant Studios


#include "GAS/Abilities/GAAnimated.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Player/Avatar.h"
#include "Combat/Artillery/LinearSkillshot.h"
#include "Player/RTPlayerState.h"
#include "GAS/Tasks/PlayMontageAndWaitForEvent.h"

UGAAnimated::UGAAnimated()
{
}

void UGAAnimated::SetSelfTags(const bool bApply) const
{
	FGameplayTag Casting = FGameplayTag::RequestGameplayTag("States.Casting");
	if(bApply)
	{
		FGameplayTagContainer Tags;
		GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);
		for(auto SelfTag : SelfTags)
		{
			if(!Tags.HasTag(SelfTag))
			{
				GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(SelfTag);
				GetAbilitySystemComponentFromActorInfo()->AddReplicatedLooseGameplayTag(SelfTag);
			}
		}
	}
	else
	{
		FGameplayTagContainer Tags;
		GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);
		for(auto SelfTag : SelfTags)
		{
			if(SelfTag == Casting)
			{
				continue;
			}
			if(Tags.HasTag(SelfTag))
			{
				GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(SelfTag);
				GetAbilitySystemComponentFromActorInfo()->RemoveReplicatedLooseGameplayTag(SelfTag);
			}
		}
		if(Tags.HasTag(Casting))
		{
			GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(Casting);
			GetAbilitySystemComponentFromActorInfo()->RemoveReplicatedLooseGameplayTag(Casting);
		}
	}
}

void UGAAnimated::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefaultAndEndAbility();
}

void UGAAnimated::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefaultAndEndAbility(true);
}

void UGAAnimated::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefaultAndEndAbility(true);
}

void UGAAnimated::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefaultAndEndAbility();	
}

void UGAAnimated::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo());
	ARTPlayerState* Owner = Cast<ARTPlayerState>(GetOwningActorFromActorInfo());

	SetSelfTags(true);
	
	FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData,0);

	FVector Loc = Avatar->GetActorLocation();
	Loc.Z = 0;
	FVector Direction = (HitResult.Location - Loc).GetSafeNormal();
	Avatar->SetRotationLock(true, Direction);

	SetMouseWorldLocation(HitResult.Location);
	
	BindAnimations();
}

void UGAAnimated::ReturnToDefault() const
{
	if(AAvatar* Avatar = Cast<AAvatar>(GetAvatarActorFromActorInfo()))
	{
		Avatar->C_SetRotationLock(false);
		SetSelfTags(false);
	}
}

void UGAAnimated::BindAnimations(float PlayRate)
{
	UPlayMontageAndWaitForEvent* Task = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, Montage,SpawnTags, PlayRate * MontagePlayRate, NAME_None, false, 1.0f);
	Task->OnCompleted.AddDynamic(this, &UGAAnimated::OnCompleted);
	Task->OnCancelled.AddDynamic(this, &UGAAnimated::OnCancelled);
	Task->OnInterrupted.AddDynamic(this, &UGAAnimated::OnInterrupted);
	Task->OnBlendOut.AddDynamic(this, &UGAAnimated::OnBlendOut);
	Task->EventReceived.AddDynamic(this, &UGAAnimated::OnEventReceived);
	Task->ReadyForActivation();
}

void UGAAnimated::BindAnimations()
{
	UPlayMontageAndWaitForEvent* Task = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, Montage,SpawnTags, MontagePlayRate, NAME_None, false, 1.0f);
	Task->OnCompleted.AddDynamic(this, &UGAAnimated::OnCompleted);
	Task->OnCancelled.AddDynamic(this, &UGAAnimated::OnCancelled);
	Task->OnInterrupted.AddDynamic(this, &UGAAnimated::OnInterrupted);
	Task->OnBlendOut.AddDynamic(this, &UGAAnimated::OnBlendOut);
	Task->EventReceived.AddDynamic(this, &UGAAnimated::OnEventReceived);
	Task->ReadyForActivation();
}

void UGAAnimated::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAAnimated::ReturnToDefaultAndEndAbility(bool bWasCancelled)
{
	ReturnToDefault();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void UGAAnimated::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimCompleted(EventTag, EventData);
}

void UGAAnimated::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimCancelled(EventTag, EventData);
}

void UGAAnimated::OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimInterrupted(EventTag, EventData);
}

void UGAAnimated::OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimBlendOut(EventTag, EventData);
}

void UGAAnimated::OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	OnAnimEventReceived(EventTag, EventData);
}