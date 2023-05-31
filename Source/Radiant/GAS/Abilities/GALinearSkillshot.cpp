// Copyright Radiant Studios


#include "GAS/Abilities/GALinearSkillshot.h"

#include "Character/Hero.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Combat/Artillery/LinearSkillshot.h"
#include "Player/RTPlayerState.h"
#include "Util/PlayMontageAndWaitForEvent.h"
#include "Util/Util.h"

UGALinearSkillshot::UGALinearSkillshot() : UGAAnimated()
{
	
}

void UGALinearSkillshot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
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

		Projectile->SetOwner(Avatar);
		Projectile->FinishSpawning(SpawnTransform);
	}
}