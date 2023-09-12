// Copyright Radiant Studios


#include "GATowerAttack.h"

#include "Tower.h"
#include "Combat/Artillery/HeatSeeking.h"
#include "Util/Util.h"

void UGATowerAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwningActorFromActorInfo();
		ATower* Tower = Cast<ATower>(GetAvatarActorFromActorInfo());
		FTransform Transform = Tower->GetTransform();
		Transform.AddToTranslation(Tower->GetGemSocketOffset());
		AHeatSeeking* Projectile = GetWorld()->SpawnActorDeferred<AHeatSeeking>(
			ProjectileClass, Transform, GetOwningActorFromActorInfo(), nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->SetTarget(Tower->GetTarget());
		Projectile->FinishSpawning(Transform);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}
