// Copyright Radiant Studios


#include "GAS/Abilities/BasicAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Avatar.h"
#include "Util/PlayMontageAndWaitForEvent.h"
#include "Util/Util.h"

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
	
	BindAnimations();
}
