// Copyright Radiant Studios


#include "GAS/Abilities/GAAreaOfEffect.h"

#include "Character/Hero.h"
#include "Combat/Artillery/AreaOfEffect.h"
#include "Player/RTPlayerState.h"
#include "Util/Util.h"

UGAAreaOfEffect::UGAAreaOfEffect()
{
	
}

void UGAAreaOfEffect::SpawnAreaOfEffect()
{
	if(HasAuthority(&CurrentActivationInfo))
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		FVector SpawnLocation = FVector::ZeroVector;
		switch (SpawnBehavior)
		{
		case ESpawnBehavior::AtAvatarLocation:
			SpawnLocation = Avatar->GetActorLocation();
			break;
		case ESpawnBehavior::AtRangeLocation:
			SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * SpawnRange;
			break;
		case ESpawnBehavior::AtMouseLocation:
			FVector AvatarLocation = Avatar->GetActorLocation();
			AvatarLocation.Z = 0.f;
			FVector Direction = GetMouseWorldLocation() - AvatarLocation;
			if(Direction.Length() > SpawnRange)
			{
				Direction = Direction.GetSafeNormal() * SpawnRange;
				SpawnLocation = AvatarLocation + Direction;
			}
			else
			{
				SpawnLocation = GetMouseWorldLocation();
			}
		}
		FRotator Rotation = bMatchAvatarRotation ? Avatar->GetActorRotation() : SpawnRotation;
		FTransform SpawnTransform = FTransform(Rotation, SpawnLocation);
		
		AAreaOfEffect* AreaOfEffect = GetWorld()->SpawnActorDeferred<AAreaOfEffect>(
			SpawnClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(Avatar)
			);

		AreaOfEffect->SetAvatar(Cast<AHero>(Avatar));
		AreaOfEffect->FinishSpawning(SpawnTransform);
	}	
}

void UGAAreaOfEffect::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);	

	if(bInstantCast)
		SpawnAreaOfEffect();
}

void UGAAreaOfEffect::OnAnimEventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnAnimEventReceived(EventTag, EventData);

	if(!bInstantCast)
		SpawnAreaOfEffect();
}