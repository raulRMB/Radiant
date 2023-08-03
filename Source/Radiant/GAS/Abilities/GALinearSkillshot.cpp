// Copyright Radiant Studios


#include "GAS/Abilities/GALinearSkillshot.h"

#include "Player/Avatar.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Combat/Artillery/LinearSkillshot.h"
#include "GAS/Tasks/PlayMontageAndWaitForEvent.h"

UGALinearSkillshot::UGALinearSkillshot() : UGAAnimated()
{
	bIsCancelable = false;
}

void UGALinearSkillshot::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimCompleted(EventTag, EventData);
	if(HasAuthority(&CurrentActivationInfo))
		ReturnToDefaultAndEndAbility(false);
}

void UGALinearSkillshot::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimCancelled(EventTag, EventData);
	if(HasAuthority(&CurrentActivationInfo))
		ReturnToDefaultAndEndAbility(false);
}

void UGALinearSkillshot::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimInterrupted(EventTag, EventData);
	if(HasAuthority(&CurrentActivationInfo))
		ReturnToDefaultAndEndAbility(false);
}

void UGALinearSkillshot::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimBlendOut(EventTag, EventData);
	if(HasAuthority(&CurrentActivationInfo))
		ReturnToDefaultAndEndAbility(false);
}

void UGALinearSkillshot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	bIsCancelable = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGALinearSkillshot::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);
	
	if(HasAuthority(&CurrentActivationInfo))
	{
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

		AActor* Avatar = GetAvatarActorFromActorInfo();
		FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * SpawnRange;
		FVector TargetLocation = GetMouseWorldLocation();
		TargetLocation.Z = Avatar->GetActorLocation().Z;
		FRotator Rotation = (TargetLocation - Avatar->GetActorLocation()).Rotation();
		FTransform SpawnTransform = FTransform(Rotation, SpawnLocation);
		
		ALinearSkillshot* Projectile = GetWorld()->SpawnActorDeferred<ALinearSkillshot>(
			SpawnClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(Avatar)
			);
		Projectile->FinishSpawning(SpawnTransform);
	}

	ReturnToDefaultAndEndAbility(false);
}