// Copyright Radiant Studios


#include "Characters/Avatars/CroudControl/GAJook.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Jook.h"
#include "Characters/RTCharacter.h"
#include "GAS/Tasks/AbilityTask_DisplaceTarget.h"
#include "Util/Util.h"

void UGAJook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SetCanBeCanceled(false);
}

void UGAJook::OnTargetHit(AActor* NewTarget)
{
	Target = NewTarget;
	
	FDisplaceTargetData DisplaceTargetData;	
	DisplaceTargetData.Target = Target;
	DisplaceTargetData.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	DisplaceTargetData.Duration = FVector::Dist(Target->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation()) / DisplaceSpeed;
	
	UAbilityTask_DisplaceTarget* Task = UAbilityTask_DisplaceTarget::DisplaceTarget(this,"DisplaceTarget", DisplaceTargetData);

	if(ARTCharacter* Character = Cast<ARTCharacter>(Target))
	{
		Character->M_SetIgnoreWalls(true);
	}
	
	if(IsValid(this))
	{
		if(IAbilitySystemInterface* Interface =Cast<IAbilitySystemInterface>(Target))
		{
			if(HasAuthority(&CurrentActivationInfo))
			{
				Interface->GetAbilitySystemComponent()->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag("States.Movement.Stopped"));
			}
		}
		Task->OnTimedOut.AddDynamic(this, &UGAJook::OnFinished);
		Task->OnTimedOutAndDestinationReached.AddDynamic(this, &UGAJook::OnFinished);
		Task->ReadyForActivation();
	}
}

void UGAJook::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(HasAuthority(&CurrentActivationInfo))
	{
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

		AActor* Avatar = GetAvatarActorFromActorInfo();
		FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * SpawnRange;
		FVector TargetLocation = GetMouseWorldLocation();
		TargetLocation.Z = Avatar->GetActorLocation().Z;
		FRotator Rotation = (TargetLocation - Avatar->GetActorLocation()).Rotation();
		FTransform SpawnTransform = FTransform(Rotation, SpawnLocation);
		
		AJook* Jook = GetWorld()->SpawnActorDeferred<AJook>(
			SpawnClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(Avatar)
			);
		Jook->SetAbility(this);
		Jook->FinishSpawning(SpawnTransform);
	}
}

void UGAJook::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefault();
}

void UGAJook::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefault();
}

void UGAJook::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefault();
}

void UGAJook::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ReturnToDefault();
}

void UGAJook::OnFinished()
{	
	ReturnToDefaultAndEndAbility();
	if(IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Target))
	{
		if(HasAuthority(&CurrentActivationInfo))
		{
			Interface->GetAbilitySystemComponent()->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag("States.Movement.Stopped"));
			if(ARTCharacter* Character = Cast<ARTCharacter>(Target))
			{
				Character->M_SetIgnoreWalls(false);
			}
		}
	}
}


