// Copyright Radiant Studios


#include "Characters/Avatars/CroudControl/GAHook.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Hook.h"
#include "Characters/RTCharacter.h"
#include "GAS/Tasks/AbilityTask_DisplaceTarget.h"
#include "Util/Util.h"

void UGAHook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SetCanBeCanceled(false);
}

void UGAHook::OnTargetHit(AActor* NewTarget)
{
	Target = NewTarget;
	
	FDisplaceTargetData DisplaceTargetData;	
	DisplaceTargetData.Target = Target;
	FVector TargetLocation = GetAvatarActorFromActorInfo()->GetActorForwardVector() * 225.f;
	TargetLocation += GetAvatarActorFromActorInfo()->GetActorLocation();
	DisplaceTargetData.Location = TargetLocation;
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
		Task->OnTimedOut.AddDynamic(this, &UGAHook::OnFinished);
		Task->OnTimedOutAndDestinationReached.AddDynamic(this, &UGAHook::OnFinished);
		Task->ReadyForActivation();
	}
}

void UGAHook::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(HasAuthority(&CurrentActivationInfo))
	{
		SetCanBeCanceled(true);
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

		AActor* Avatar = GetAvatarActorFromActorInfo();
		FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * SpawnRange;
		FVector TargetLocation = GetMouseWorldLocation();
		TargetLocation.Z = Avatar->GetActorLocation().Z;
		FRotator Rotation = (TargetLocation - Avatar->GetActorLocation()).Rotation();
		FTransform SpawnTransform = FTransform(Rotation, SpawnLocation);
		
		AHook* Hook = GetWorld()->SpawnActorDeferred<AHook>(
			SpawnClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(Avatar)
			);
		Hook->SetAbility(this);
		if(Cast<IHooker>(Avatar))
		{
			Hook->AttachToHooker(Avatar);
		}
		Hook->FinishSpawning(SpawnTransform);
	}
}

void UGAHook::OnAnimBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	SetCanBeCanceled(true);
	ReturnToDefault();
}

void UGAHook::OnAnimCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	SetCanBeCanceled(true);
	ReturnToDefault();
}

void UGAHook::OnAnimCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	SetCanBeCanceled(true);
	ReturnToDefault();
}

void UGAHook::OnAnimInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	SetCanBeCanceled(true);
	ReturnToDefault();
}

void UGAHook::OnFinished()
{	
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
	ReturnToDefaultAndEndAbility();
}


