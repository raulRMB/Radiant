// Copyright Radiant Studios


#include "GAS/Abilities/GALinearSkillshot.h"

#include "Character/Hero.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RTPlayerState.h"
#include "Util/PlayMontageAndWaitForEvent.h"

UGALinearSkillshot::UGALinearSkillshot()
{
	FAbilityTriggerData AbilityTrigger;
	AbilityTrigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTrigger.TriggerTag = FGameplayTag::RequestGameplayTag("Ability.Combat.Fireball");
	AbilityTriggers.Add(AbilityTrigger);
}

void UGALinearSkillshot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AHero* Avatar = Cast<AHero>(GetAvatarActorFromActorInfo());
	ARTPlayerState* Owner = Cast<ARTPlayerState>(GetOwningActorFromActorInfo());

	FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData,0);

	FVector Loc = Avatar->GetActorLocation();
	Loc.Z = 0;
	FVector Direction = (HitResult.Location - Loc).GetSafeNormal();
	Avatar->SetRotationLock(true, Direction);

	MouseWorldLocation = HitResult.Location;

	UPlayMontageAndWaitForEvent* Task = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, Montage, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	// UPlayMontageAndWaitForEvent* Task = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, FName("Fireball"), Montage, FGameplayTagContainer(), MontagePlayRate);
	Task->OnCompleted.AddDynamic(this, &UGALinearSkillshot::OnCompleted);
	Task->OnCancelled.AddDynamic(this, &UGALinearSkillshot::OnCancelled);
	Task->OnInterrupted.AddDynamic(this, &UGALinearSkillshot::OnInterrupted);
	Task->OnBlendOut.AddDynamic(this, &UGALinearSkillshot::OnBlendOut);
	Task->EventReceived.AddDynamic(this, &UGALinearSkillshot::OnEventReceived);
	Task->ReadyForActivation();
}

void UGALinearSkillshot::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGALinearSkillshot::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGALinearSkillshot::OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGALinearSkillshot::OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGALinearSkillshot::OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(HasAuthority(&CurrentActivationInfo))
	{
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

		AActor* Avatar = GetAvatarActorFromActorInfo();
		FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * SpawnRange;
		FVector TargetLocation = MouseWorldLocation;
		TargetLocation.Z = Avatar->GetActorLocation().Z;
		FRotator Rotation = (TargetLocation - Avatar->GetActorLocation()).Rotation();
		FTransform SpawnTransform = FTransform(Rotation, SpawnLocation);
		
		GetWorld()->SpawnActor(SpawnActor, &SpawnTransform);
	}
}
