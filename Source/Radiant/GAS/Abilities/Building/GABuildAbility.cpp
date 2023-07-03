// Copyright Radiant Studios


#include "GAS/Abilities/Building/GABuildAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Building/Building.h"

void UGABuildAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(HasAuthority(&CurrentActivationInfo))
	{		
		if(ITeamMember* TeamMember = Cast<ITeamMember>(GetOwningActorFromActorInfo()))
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData,0);
			FVector Dir = HitResult.Location - GetAvatarActorFromActorInfo()->GetActorLocation();
			FTransform SpawnTransform;
			if(Dir.Length() > MaxRange)
			{
				Dir = Dir.GetSafeNormal() * MaxRange;
				SpawnTransform = FTransform(Dir + GetAvatarActorFromActorInfo()->GetActorLocation());
			}
			else
			{
				SpawnTransform = FTransform(HitResult.Location);
			}
			
			
			ABuilding* Building = GetWorld()->SpawnActorDeferred<ABuilding>(BuildingClass, SpawnTransform);
			Building->SetOwner(GetOwningActorFromActorInfo());
			Building->S_SetTeamId(TeamMember->GetTeamId());
			Building->FinishSpawning(SpawnTransform);
		}
	}
}
